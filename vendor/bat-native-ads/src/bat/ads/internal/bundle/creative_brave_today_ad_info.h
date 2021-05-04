/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_BUNDLE_CREATIVE_BRAVE_TODAY_AD_INFO_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_BUNDLE_CREATIVE_BRAVE_TODAY_AD_INFO_H_

#include <string>
#include <vector>

#include "bat/ads/internal/bundle/creative_ad_info.h"

namespace ads {

struct CreativeBraveTodayAdInfo : CreativeAdInfo {
  CreativeBraveTodayAdInfo();
  CreativeBraveTodayAdInfo(const CreativeBraveTodayAdInfo& info);
  ~CreativeBraveTodayAdInfo();

  bool operator==(const CreativeBraveTodayAdInfo& rhs) const;

  bool operator!=(const CreativeBraveTodayAdInfo& rhs) const;

  std::string title;
  std::string description;
  std::string image_url;
  std::string size;
};

using CreativeBraveTodayAdList = std::vector<CreativeBraveTodayAdInfo>;

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_BUNDLE_CREATIVE_BRAVE_TODAY_AD_INFO_H_
