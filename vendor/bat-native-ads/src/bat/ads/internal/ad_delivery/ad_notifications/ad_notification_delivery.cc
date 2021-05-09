/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ad_delivery/ad_notifications/ad_notification_delivery.h"

#include <string>
#include <vector>

#include "base/strings/stringprintf.h"
#include "bat/ads/ad_notification_info.h"
#include "bat/ads/internal/ads/ad_notifications/ad_notifications.h"
#include "bat/ads/internal/ads_client_helper.h"
#include "bat/ads/internal/client/client.h"
#include "bat/ads/internal/p2a/p2a.h"
#include "bat/ads/internal/p2a/p2a_util.h"

namespace ads {
namespace ad_notifications {

namespace {

void DeliverAd(const AdNotificationInfo& ad) {
  AdNotifications::Get()->PushBack(ad);

  AdsClientHelper::Get()->ShowNotification(ad);
}

void RecordAdImpression(const AdInfo& ad) {
  const std::string type_as_string = std::string(ad.type);

  const std::string name =
      base::StringPrintf("%s_impression", type_as_string.c_str());

  const std::vector<std::string> questions =
      p2a::CreateAdImpressionQuestionList(ad.segment);

  p2a::RecordEvent(name, questions);
}

}  // namespace

AdDelivery::AdDelivery() = default;

AdDelivery::~AdDelivery() = default;

bool AdDelivery::MaybeDeliverAd(const AdNotificationInfo& ad) {
  if (!ad.IsValid()) {
    return false;
  }

  Client::Get()->UpdateSeenAd(ad);

  RecordAdImpression(ad);

  DeliverAd(ad);

  return true;
}

}  // namespace ad_notifications
}  // namespace ads
