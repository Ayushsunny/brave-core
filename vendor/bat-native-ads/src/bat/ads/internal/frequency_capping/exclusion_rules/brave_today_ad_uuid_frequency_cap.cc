/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/exclusion_rules/brave_today_ad_uuid_frequency_cap.h"

#include <cstdint>

#include "base/strings/stringprintf.h"
#include "bat/ads/ad_info.h"
#include "bat/ads/internal/logging.h"

namespace ads {

namespace {
const uint64_t kBraveTodayAdUuidFrequencyCap = 1;
}  // namespace

BraveTodayAdUuidFrequencyCap::BraveTodayAdUuidFrequencyCap(
    const AdEventList& ad_events)
    : ad_events_(ad_events) {}

BraveTodayAdUuidFrequencyCap::~BraveTodayAdUuidFrequencyCap() = default;

bool BraveTodayAdUuidFrequencyCap::ShouldExclude(const AdInfo& ad) {
  const AdEventList filtered_ad_events = FilterAdEvents(ad_events_, ad);

  if (!DoesRespectCap(filtered_ad_events)) {
    last_message_ = base::StringPrintf(
        "uuid %s has exceeded the "
        "frequency capping for new tab page ad",
        ad.uuid.c_str());
    return true;
  }

  return false;
}

std::string BraveTodayAdUuidFrequencyCap::get_last_message() const {
  return last_message_;
}

bool BraveTodayAdUuidFrequencyCap::DoesRespectCap(
    const AdEventList& ad_events) {
  if (ad_events.size() >= kBraveTodayAdUuidFrequencyCap) {
    return false;
  }

  return true;
}

AdEventList BraveTodayAdUuidFrequencyCap::FilterAdEvents(
    const AdEventList& ad_events,
    const AdInfo& ad) const {
  AdEventList filtered_ad_events = ad_events;

  const auto iter = std::remove_if(
      filtered_ad_events.begin(), filtered_ad_events.end(),
      [&ad](const AdEventInfo& ad_event) {
        return ad_event.uuid != ad.uuid ||
               ad_event.confirmation_type != ConfirmationType::kViewed ||
               ad_event.type != AdType::kBraveTodayAd;
      });

  filtered_ad_events.erase(iter, filtered_ad_events.end());

  return filtered_ad_events;
}

}  // namespace ads
