/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ADS_BRAVE_TODAY_ADS_BRAVE_TODAY_AD_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ADS_BRAVE_TODAY_ADS_BRAVE_TODAY_AD_H_

#include <string>

#include "bat/ads/internal/ad_events/ad_event_info.h"
#include "bat/ads/internal/ads/brave_today_ads/brave_today_ad_observer.h"
#include "bat/ads/mojom.h"

namespace ads {

struct BraveTodayAdInfo;

class BraveTodayAd : public BraveTodayAdObserver {
 public:
  BraveTodayAd();

  ~BraveTodayAd() override;

  void AddObserver(BraveTodayAdObserver* observer);
  void RemoveObserver(BraveTodayAdObserver* observer);

  void FireEvent(const std::string& uuid,
                 const std::string& creative_instance_id,
                 const BraveTodayAdEventType event_type);

 private:
  base::ObserverList<BraveTodayAdObserver> observers_;

  bool ShouldFireEvent(const BraveTodayAdInfo& ad,
                       const AdEventList& ad_events);

  void FireEvent(const BraveTodayAdInfo& ad,
                 const std::string& uuid,
                 const std::string& creative_instance_id,
                 const BraveTodayAdEventType event_type);

  void NotifyBraveTodayAdEvent(const BraveTodayAdInfo& ad,
                               const BraveTodayAdEventType event_type);

  void NotifyBraveTodayAdViewed(const BraveTodayAdInfo& ad);
  void NotifyBraveTodayAdClicked(const BraveTodayAdInfo& ad);

  void NotifyBraveTodayAdEventFailed(const std::string& uuid,
                                     const std::string& creative_instance_id,
                                     const BraveTodayAdEventType event_type);
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ADS_BRAVE_TODAY_ADS_BRAVE_TODAY_AD_H_
