/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/eligible_ads/ad_notifications/seen_advertisers.h"

#include <map>
#include <string>

#include "bat/ads/internal/client/client.h"
#include "bat/ads/internal/eligible_ads/round_robin_advertisers.h"
#include "bat/ads/internal/logging.h"

namespace ads {
namespace ad_notifications {

CreativeAdNotificationList FilterSeenAdvertisersAndRoundRobinIfNeeded(
    const CreativeAdNotificationList& ads) {
  const std::map<std::string, bool> seen_advertisers =
      Client::Get()->GetSeenAdvertisersForType(AdType::kAdNotification);

  CreativeAdNotificationList eligible_ads =
      FilterSeenAdvertisers(ads, seen_advertisers);

  if (eligible_ads.empty()) {
    BLOG(1, "All ad notification advertisers have been shown, so round robin");
    Client::Get()->ResetSeenAdvertisersForType(ads, AdType::kAdNotification);
    eligible_ads = ads;
  }

  return eligible_ads;
}

}  // namespace ad_notifications
}  // namespace ads
