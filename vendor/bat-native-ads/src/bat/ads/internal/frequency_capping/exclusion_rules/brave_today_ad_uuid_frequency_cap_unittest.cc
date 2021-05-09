/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/exclusion_rules/brave_today_ad_uuid_frequency_cap.h"

#include <vector>

#include "bat/ads/internal/frequency_capping/frequency_capping_unittest_util.h"
#include "bat/ads/internal/unittest_base.h"
#include "bat/ads/internal/unittest_util.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {

namespace {

const std::vector<std::string> kUuids = {
    "60267cee-d5bb-4a0d-baaf-91cd7f18e07e",
    "90762cee-d5bb-4a0d-baaf-61cd7f18e07e"};

}  // namespace

class BatAdsBraveTodayAdUuidFrequencyCapTest : public UnitTestBase {
 protected:
  BatAdsBraveTodayAdUuidFrequencyCapTest() = default;

  ~BatAdsBraveTodayAdUuidFrequencyCapTest() override = default;
};

TEST_F(BatAdsBraveTodayAdUuidFrequencyCapTest, AllowAdIfThereIsNoAdsHistory) {
  // Arrange
  AdInfo ad;
  ad.uuid = kUuids.at(0);

  const AdEventList ad_events;

  // Act
  BraveTodayAdUuidFrequencyCap frequency_cap(ad_events);
  const bool should_exclude = frequency_cap.ShouldExclude(ad);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BatAdsBraveTodayAdUuidFrequencyCapTest,
       AdAllowedForAdWithDifferentUuid) {
  // Arrange
  AdInfo ad_1;
  ad_1.uuid = kUuids.at(0);

  AdInfo ad_2;
  ad_2.uuid = kUuids.at(1);

  AdEventList ad_events;

  const AdEventInfo ad_event =
      GenerateAdEvent(AdType::kBraveTodayAd, ad_2, ConfirmationType::kViewed);

  ad_events.push_back(ad_event);

  // Act
  BraveTodayAdUuidFrequencyCap frequency_cap(ad_events);
  const bool should_exclude = frequency_cap.ShouldExclude(ad_1);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BatAdsBraveTodayAdUuidFrequencyCapTest,
       AdAllowedForAdWithDifferentUuidForMultipleTypes) {
  // Arrange
  AdInfo ad_1;
  ad_1.uuid = kUuids.at(0);

  AdInfo ad_2;
  ad_2.uuid = kUuids.at(1);

  AdEventList ad_events;

  const AdEventInfo ad_event_1 =
      GenerateAdEvent(AdType::kNewTabPageAd, ad_2, ConfirmationType::kViewed);
  ad_events.push_back(ad_event_1);

  const AdEventInfo ad_event_2 =
      GenerateAdEvent(AdType::kBraveTodayAd, ad_2, ConfirmationType::kViewed);
  ad_events.push_back(ad_event_2);

  const AdEventInfo ad_event_3 =
      GenerateAdEvent(AdType::kBraveTodayAd, ad_2, ConfirmationType::kViewed);
  ad_events.push_back(ad_event_3);

  // Act
  BraveTodayAdUuidFrequencyCap frequency_cap(ad_events);
  const bool should_exclude = frequency_cap.ShouldExclude(ad_1);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BatAdsBraveTodayAdUuidFrequencyCapTest, AdNotAllowedForAdWithSameUuid) {
  // Arrange
  AdInfo ad;
  ad.uuid = kUuids.at(0);

  AdEventList ad_events;

  const AdEventInfo ad_event =
      GenerateAdEvent(AdType::kBraveTodayAd, ad, ConfirmationType::kViewed);

  ad_events.push_back(ad_event);

  // Act
  BraveTodayAdUuidFrequencyCap frequency_cap(ad_events);
  const bool should_exclude = frequency_cap.ShouldExclude(ad);

  // Assert
  EXPECT_TRUE(should_exclude);
}

}  // namespace ads
