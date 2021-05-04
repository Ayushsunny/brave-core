/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/eligible_ads/brave_today_ads/eligible_brave_today_ads.h"

#include <vector>

#include "bat/ads/brave_today_ad_info.h"
#include "bat/ads/internal/ad_pacing/brave_today_ads/brave_today_ad_pacing.h"
#include "bat/ads/internal/ad_serving/ad_targeting/geographic/subdivision/subdivision_targeting.h"
#include "bat/ads/internal/ad_targeting/ad_targeting_segment_util.h"
#include "bat/ads/internal/ad_targeting/ad_targeting_values.h"
#include "bat/ads/internal/ads/brave_today_ads/brave_today_ad_frequency_capping.h"
#include "bat/ads/internal/ads_client_helper.h"
#include "bat/ads/internal/client/client.h"
#include "bat/ads/internal/database/tables/ad_events_database_table.h"
#include "bat/ads/internal/database/tables/creative_brave_today_ads_database_table.h"
// #include "bat/ads/internal/eligible_ads/brave_today_ads/seen_ads.h"
// #include "bat/ads/internal/eligible_ads/brave_today_ads/seen_advertisers.h"
#include "bat/ads/internal/features/ad_serving/ad_serving_features.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/resources/frequency_capping/anti_targeting_resource.h"

namespace ads {
namespace brave_today_ads {

namespace {

bool ShouldCapLastServedAd(const CreativeBraveTodayAdList& ads) {
  return ads.size() != 1;
}

}  // namespace

EligibleAds::EligibleAds(
    ad_targeting::geographic::SubdivisionTargeting* subdivision_targeting,
    resource::AntiTargeting* anti_targeting)
    : subdivision_targeting_(subdivision_targeting),
      anti_targeting_resource_(anti_targeting) {
  DCHECK(subdivision_targeting_);
  DCHECK(anti_targeting_resource_);
}

EligibleAds::~EligibleAds() = default;

void EligibleAds::SetLastServedAd(const CreativeAdInfo& creative_ad) {
  last_served_creative_ad_ = creative_ad;
}

void EligibleAds::GetForSegments(const SegmentList& segments,
                                 const std::string& size,
                                 GetEligibleAdsCallback callback) {
  database::table::AdEvents database_table;
  database_table.GetAll([=](const Result result, const AdEventList& ad_events) {
    if (result != Result::SUCCESS) {
      BLOG(1, "Failed to get ad events");
      callback(/* is_allowed */ false, {});
      return;
    }

    const int max_count = features::GetBrowsingHistoryMaxCount();
    const int days_ago = features::GetBrowsingHistoryDaysAgo();
    AdsClientHelper::Get()->GetBrowsingHistory(
        max_count, days_ago, [=](const BrowsingHistoryList history) {
          FrequencyCapping frequency_capping(subdivision_targeting_,
                                             anti_targeting_resource_,
                                             ad_events, history);

          if (!frequency_capping.IsAdAllowed()) {
            callback(/* is_allowed */ false, {});
            return;
          }

          if (segments.empty()) {
            GetForUntargeted(size, ad_events, history, callback);
            return;
          }

          GetForParentChildSegments(segments, size, ad_events, history,
                                    callback);
        });
  });
}

///////////////////////////////////////////////////////////////////////////////

void EligibleAds::GetForParentChildSegments(
    const SegmentList& segments,
    const std::string& size,
    const AdEventList& ad_events,
    const BrowsingHistoryList& history,
    GetEligibleAdsCallback callback) const {
  DCHECK(!segments.empty());

  BLOG(1, "Get eligible ads for parent-child segments:");
  for (const auto& segment : segments) {
    BLOG(1, "  " << segment);
  }

  database::table::CreativeBraveTodayAds database_table;
  database_table.GetForSegments(
      segments, [=](const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& ads) {
        CreativeBraveTodayAdList eligible_ads =
            FilterIneligibleAds(ads, ad_events, history);

        if (eligible_ads.empty()) {
          BLOG(1, "No eligible ads for parent-child segments");
          GetForParentSegments(segments, size, ad_events, history, callback);
          return;
        }

        callback(/* is_allowed */ true, eligible_ads);
      });
}

void EligibleAds::GetForParentSegments(const SegmentList& segments,
                                       const std::string& size,
                                       const AdEventList& ad_events,
                                       const BrowsingHistoryList& history,
                                       GetEligibleAdsCallback callback) const {
  DCHECK(!segments.empty());

  const SegmentList parent_segments = GetParentSegments(segments);
  if (parent_segments == segments) {
    return;
  }

  BLOG(1, "Get eligible ads for parent segments:");
  for (const auto& parent_segment : parent_segments) {
    BLOG(1, "  " << parent_segment);
  }

  database::table::CreativeBraveTodayAds database_table;
  database_table.GetForSegments(
      parent_segments, [=](const Result result, const SegmentList& segments,
                           const CreativeBraveTodayAdList& ads) {
        CreativeBraveTodayAdList eligible_ads =
            FilterIneligibleAds(ads, ad_events, history);

        if (eligible_ads.empty()) {
          BLOG(1, "No eligible ads for parent segments");
          GetForUntargeted(size, ad_events, history, callback);
          return;
        }

        callback(/* is_allowed */ true, eligible_ads);
      });
}

void EligibleAds::GetForUntargeted(const std::string& size,
                                   const AdEventList& ad_events,
                                   const BrowsingHistoryList& history,
                                   GetEligibleAdsCallback callback) const {
  BLOG(1, "Get eligble ads for untargeted segment");

  const std::vector<std::string> segments = {ad_targeting::kUntargeted};

  database::table::CreativeBraveTodayAds database_table;
  database_table.GetForSegments(
      segments, [=](const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& ads) {
        CreativeBraveTodayAdList eligible_ads =
            FilterIneligibleAds(ads, ad_events, history);

        if (eligible_ads.empty()) {
          BLOG(1, "No eligible ads for untargeted segment");
        }

        callback(/* is_allowed */ true, eligible_ads);
      });
}

CreativeBraveTodayAdList EligibleAds::FilterIneligibleAds(
    const CreativeBraveTodayAdList& ads,
    const AdEventList& ad_events,
    const BrowsingHistoryList& history) const {
  CreativeBraveTodayAdList eligible_ads = ads;
  if (eligible_ads.empty()) {
    return eligible_ads;
  }

  // TODO(tmancey): Implement round robin
  // eligible_ads = FilterSeenAdvertisersAndRoundRobinIfNeeded(eligible_ads);

  // TODO(tmancey): Implement round robin
  // eligible_ads = FilterSeenAdsAndRoundRobinIfNeeded(eligible_ads);

  eligible_ads = ApplyFrequencyCapping(
      eligible_ads,
      ShouldCapLastServedAd(ads) ? last_served_creative_ad_ : CreativeAdInfo(),
      ad_events, history);

  eligible_ads = PaceAds(eligible_ads);

  return eligible_ads;
}

CreativeBraveTodayAdList EligibleAds::ApplyFrequencyCapping(
    const CreativeBraveTodayAdList& ads,
    const CreativeAdInfo& last_served_creative_ad,
    const AdEventList& ad_events,
    const BrowsingHistoryList& history) const {
  CreativeBraveTodayAdList eligible_ads = ads;

  FrequencyCapping frequency_capping(
      subdivision_targeting_, anti_targeting_resource_, ad_events, history);

  const auto iter = std::remove_if(
      eligible_ads.begin(), eligible_ads.end(),
      [&frequency_capping, &last_served_creative_ad](CreativeAdInfo& ad) {
        return frequency_capping.ShouldExcludeAd(ad) ||
               ad.creative_instance_id ==
                   last_served_creative_ad.creative_instance_id;
      });

  eligible_ads.erase(iter, eligible_ads.end());

  return eligible_ads;
}

}  // namespace brave_today_ads
}  // namespace ads
