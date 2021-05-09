/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ads/brave_today_ads/brave_today_ad.h"

#include "bat/ads/brave_today_ad_info.h"
#include "bat/ads/internal/ad_events/brave_today_ads/brave_today_ad_event_factory.h"
#include "bat/ads/internal/ads/brave_today_ads/brave_today_ad_frequency_capping.h"
#include "bat/ads/internal/bundle/creative_brave_today_ad_info.h"
#include "bat/ads/internal/database/tables/ad_events_database_table.h"
#include "bat/ads/internal/database/tables/creative_brave_today_ads_database_table.h"
#include "bat/ads/internal/logging.h"

namespace ads {

namespace {

BraveTodayAdInfo CreateBraveTodayAd(const std::string& uuid,
                                    const CreativeBraveTodayAdInfo& ad) {
  BraveTodayAdInfo brave_today_ad;

  brave_today_ad.type = AdType::kBraveTodayAd;
  brave_today_ad.uuid = uuid;
  brave_today_ad.creative_instance_id = ad.creative_instance_id;
  brave_today_ad.creative_set_id = ad.creative_set_id;
  brave_today_ad.campaign_id = ad.campaign_id;
  brave_today_ad.advertiser_id = ad.advertiser_id;
  brave_today_ad.segment = ad.segment;
  brave_today_ad.target_url = ad.target_url;
  brave_today_ad.title = ad.title;
  brave_today_ad.description = ad.description;

  return brave_today_ad;
}

}  // namespace

BraveTodayAd::BraveTodayAd() = default;

BraveTodayAd::~BraveTodayAd() = default;

void BraveTodayAd::AddObserver(BraveTodayAdObserver* observer) {
  DCHECK(observer);
  observers_.AddObserver(observer);
}

void BraveTodayAd::RemoveObserver(BraveTodayAdObserver* observer) {
  DCHECK(observer);
  observers_.RemoveObserver(observer);
}

void BraveTodayAd::FireEvent(const std::string& uuid,
                             const std::string& creative_instance_id,
                             const BraveTodayAdEventType event_type) {
  if (uuid.empty() || creative_instance_id.empty()) {
    BLOG(1, "Failed to fire Brave Today ad event for uuid "
                << uuid << " and creative instance id "
                << creative_instance_id);

    NotifyBraveTodayAdEventFailed(uuid, creative_instance_id, event_type);

    return;
  }

  database::table::CreativeBraveTodayAds database_table;
  database_table.GetForCreativeInstanceId(
      creative_instance_id,
      [=](const Result result, const std::string& creative_instance_id,
          const CreativeBraveTodayAdInfo& creative_brave_today_ad) {
        if (result != SUCCESS) {
          BLOG(1, "Failed to fire Brave Today ad event for uuid");

          NotifyBraveTodayAdEventFailed(uuid, creative_instance_id, event_type);

          return;
        }

        const BraveTodayAdInfo ad =
            CreateBraveTodayAd(uuid, creative_brave_today_ad);

        FireEvent(ad, uuid, creative_instance_id, event_type);
      });
}

///////////////////////////////////////////////////////////////////////////////

bool BraveTodayAd::ShouldFireEvent(const BraveTodayAdInfo& ad,
                                   const AdEventList& ad_events) {
  brave_today_ads::FrequencyCapping frequency_capping(ad_events);

  if (!frequency_capping.IsAdAllowed()) {
    return false;
  }

  if (frequency_capping.ShouldExcludeAd(ad)) {
    return false;
  }

  return true;
}

void BraveTodayAd::FireEvent(const BraveTodayAdInfo& ad,
                             const std::string& uuid,
                             const std::string& creative_instance_id,
                             const BraveTodayAdEventType event_type) {
  database::table::AdEvents database_table;
  database_table.GetAll([=](const Result result, const AdEventList& ad_events) {
    if (result != Result::SUCCESS) {
      BLOG(1, "Brave Today ad: Failed to get ad events");

      NotifyBraveTodayAdEventFailed(uuid, creative_instance_id, event_type);

      return;
    }

    if (event_type == BraveTodayAdEventType::kViewed &&
        !ShouldFireEvent(ad, ad_events)) {
      BLOG(1, "Brave Today ad: Not allowed");

      NotifyBraveTodayAdEventFailed(uuid, creative_instance_id, event_type);

      return;
    }

    const auto ad_event = brave_today_ads::AdEventFactory::Build(event_type);
    ad_event->FireEvent(ad);

    NotifyBraveTodayAdEvent(ad, event_type);
  });
}

void BraveTodayAd::NotifyBraveTodayAdEvent(
    const BraveTodayAdInfo& ad,
    const BraveTodayAdEventType event_type) {
  switch (event_type) {
    case BraveTodayAdEventType::kViewed: {
      NotifyBraveTodayAdViewed(ad);
      break;
    }

    case BraveTodayAdEventType::kClicked: {
      NotifyBraveTodayAdClicked(ad);
      break;
    }
  }
}

void BraveTodayAd::NotifyBraveTodayAdViewed(const BraveTodayAdInfo& ad) {
  for (BraveTodayAdObserver& observer : observers_) {
    observer.OnBraveTodayAdViewed(ad);
  }
}

void BraveTodayAd::NotifyBraveTodayAdClicked(const BraveTodayAdInfo& ad) {
  for (BraveTodayAdObserver& observer : observers_) {
    observer.OnBraveTodayAdClicked(ad);
  }
}

void BraveTodayAd::NotifyBraveTodayAdEventFailed(
    const std::string& uuid,
    const std::string& creative_instance_id,
    const BraveTodayAdEventType event_type) {
  for (BraveTodayAdObserver& observer : observers_) {
    observer.OnBraveTodayAdEventFailed(uuid, creative_instance_id, event_type);
  }
}

}  // namespace ads
