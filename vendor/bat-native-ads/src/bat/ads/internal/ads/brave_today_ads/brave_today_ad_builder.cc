/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ads/brave_today_ads/brave_today_ad_builder.h"

#include "base/guid.h"
#include "bat/ads/brave_today_ad_info.h"
#include "bat/ads/internal/bundle/creative_brave_today_ad_info.h"

namespace ads {

BraveTodayAdInfo BuildBraveTodayAd(
    const CreativeBraveTodayAdInfo& creative_brave_today_ad) {
  const std::string uuid = base::GenerateGUID();
  return BuildBraveTodayAd(creative_brave_today_ad, uuid);
}

BraveTodayAdInfo BuildBraveTodayAd(
    const CreativeBraveTodayAdInfo& creative_brave_today_ad,
    const std::string& uuid) {
  BraveTodayAdInfo brave_today_ad;

  brave_today_ad.type = AdType::kBraveTodayAd;
  brave_today_ad.uuid = uuid;
  brave_today_ad.creative_instance_id =
      creative_brave_today_ad.creative_instance_id;
  brave_today_ad.creative_set_id = creative_brave_today_ad.creative_set_id;
  brave_today_ad.campaign_id = creative_brave_today_ad.campaign_id;
  brave_today_ad.advertiser_id = creative_brave_today_ad.advertiser_id;
  brave_today_ad.segment = creative_brave_today_ad.segment;
  brave_today_ad.title = creative_brave_today_ad.title;
  brave_today_ad.description = creative_brave_today_ad.description;
  brave_today_ad.image_url = creative_brave_today_ad.image_url;
  brave_today_ad.size = creative_brave_today_ad.size;
  brave_today_ad.target_url = creative_brave_today_ad.target_url;

  return brave_today_ad;
}

}  // namespace ads
