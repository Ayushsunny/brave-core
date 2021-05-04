/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ELIGIBLE_ADS_BRAVE_TODAY_ADS_ELIGIBLE_BRAVE_TODAY_ADS_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ELIGIBLE_ADS_BRAVE_TODAY_ADS_ELIGIBLE_BRAVE_TODAY_ADS_H_

#include <string>

#include "bat/ads/internal/ad_events/ad_event_info.h"
#include "bat/ads/internal/ad_targeting/ad_targeting_segment.h"
#include "bat/ads/internal/bundle/creative_brave_today_ad_info.h"
#include "bat/ads/internal/frequency_capping/frequency_capping_aliases.h"

namespace ads {

namespace ad_targeting {
namespace geographic {
class SubdivisionTargeting;
}  // namespace geographic
}  // namespace ad_targeting

namespace resource {
class AntiTargeting;
}  // namespace resource

namespace brave_today_ads {

using GetEligibleAdsCallback =
    std::function<void(const bool, const CreativeBraveTodayAdList&)>;

class EligibleAds {
 public:
  EligibleAds(
      ad_targeting::geographic::SubdivisionTargeting* subdivision_targeting,
      resource::AntiTargeting* anti_targeting);

  ~EligibleAds();

  void SetLastServedAd(const CreativeAdInfo& creative_ad);

  void GetForSegments(const SegmentList& segments,
                      const std::string& size,
                      GetEligibleAdsCallback callback);

 private:
  ad_targeting::geographic::SubdivisionTargeting*
      subdivision_targeting_;  // NOT OWNED

  resource::AntiTargeting* anti_targeting_resource_;  // NOT OWNED

  CreativeAdInfo last_served_creative_ad_;

  void GetForParentChildSegments(const SegmentList& segments,
                                 const std::string& size,
                                 const AdEventList& ad_events,
                                 const BrowsingHistoryList& history,
                                 GetEligibleAdsCallback callback) const;

  void GetForParentSegments(const SegmentList& segments,
                            const std::string& size,
                            const AdEventList& ad_events,
                            const BrowsingHistoryList& history,
                            GetEligibleAdsCallback callback) const;

  void GetForUntargeted(const std::string& size,
                        const AdEventList& ad_events,
                        const BrowsingHistoryList& history,
                        GetEligibleAdsCallback callback) const;

  CreativeBraveTodayAdList FilterIneligibleAds(
      const CreativeBraveTodayAdList& ads,
      const AdEventList& ad_events,
      const BrowsingHistoryList& history) const;

  CreativeBraveTodayAdList ApplyFrequencyCapping(
      const CreativeBraveTodayAdList& ads,
      const CreativeAdInfo& last_served_creative_ad,
      const AdEventList& ad_events,
      const BrowsingHistoryList& history) const;
};

}  // namespace brave_today_ads
}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ELIGIBLE_ADS_BRAVE_TODAY_ADS_ELIGIBLE_BRAVE_TODAY_ADS_H_
