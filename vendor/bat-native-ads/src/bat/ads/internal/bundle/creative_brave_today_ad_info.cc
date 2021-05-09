/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/bundle/creative_brave_today_ad_info.h"

namespace ads {

CreativeBraveTodayAdInfo::CreativeBraveTodayAdInfo() = default;

CreativeBraveTodayAdInfo::~CreativeBraveTodayAdInfo() = default;

bool CreativeBraveTodayAdInfo::operator==(
    const CreativeBraveTodayAdInfo& rhs) const {
  return title == rhs.title && description == rhs.description;
}

bool CreativeBraveTodayAdInfo::operator!=(
    const CreativeBraveTodayAdInfo& rhs) const {
  return !(*this == rhs);
}

}  // namespace ads
