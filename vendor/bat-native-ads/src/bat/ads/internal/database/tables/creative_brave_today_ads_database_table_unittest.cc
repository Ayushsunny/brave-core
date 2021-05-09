/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/database/tables/creative_brave_today_ads_database_table.h"

#include "bat/ads/internal/container_util.h"
#include "bat/ads/internal/unittest_base.h"
#include "bat/ads/internal/unittest_util.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {

class BatAdsCreativeBraveTodayAdsDatabaseTableTest : public UnitTestBase {
 protected:
  BatAdsCreativeBraveTodayAdsDatabaseTableTest()
      : database_table_(
            std::make_unique<database::table::CreativeBraveTodayAds>()) {}

  ~BatAdsCreativeBraveTodayAdsDatabaseTableTest() override = default;

  void Save(const CreativeBraveTodayAdList& creative_brave_today_ads) {
    database_table_->Save(creative_brave_today_ads, [](const Result result) {
      ASSERT_EQ(Result::SUCCESS, result);
    });
  }

  std::unique_ptr<database::table::CreativeBraveTodayAds> database_table_;
};

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       SaveEmptyCreativeBraveTodayAds) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads = {};

  // Act
  Save(creative_brave_today_ads);

  // Assert
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       SaveCreativeBraveTodayAds) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPastAsTimestamp();
  info_1.end_at_timestamp = DistantFutureAsTimestamp();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.segment = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = {"US"};
  info_1.target_url = "https://brave.com";
  info_1.title = "Test Ad 1 Title";
  info_1.description = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_brave_today_ads.push_back(info_1);

  CreativeBraveTodayAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPastAsTimestamp();
  info_2.end_at_timestamp = DistantFutureAsTimestamp();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.segment = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = {"US"};
  info_2.target_url = "https://brave.com";
  info_2.title = "Test Ad 2 Title";
  info_2.description = "Test Ad 2 Body";
  info_2.ptr = 0.8;
  creative_brave_today_ads.push_back(info_2);

  // Act
  Save(creative_brave_today_ads);

  // Assert
  const CreativeBraveTodayAdList expected_creative_brave_today_ads =
      creative_brave_today_ads;

  const SegmentList segments = {"Technology & Computing-Software"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       SaveCreativeBraveTodayAdsInBatches) {
  // Arrange
  database_table_->set_batch_size(2);

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeBraveTodayAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPastAsTimestamp();
  info_1.end_at_timestamp = DistantFutureAsTimestamp();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.segment = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = {"US"};
  info_1.target_url = "https://brave.com";
  info_1.title = "Test Ad 1 Title";
  info_1.description = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_brave_today_ads.push_back(info_1);

  CreativeBraveTodayAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPastAsTimestamp();
  info_2.end_at_timestamp = DistantFutureAsTimestamp();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.segment = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = {"US"};
  info_2.target_url = "https://brave.com";
  info_2.title = "Test Ad 2 Title";
  info_2.description = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_brave_today_ads.push_back(info_2);

  CreativeBraveTodayAdInfo info_3;
  info_3.creative_instance_id = "a1ac44c2-675f-43e6-ab6d-500614cafe63";
  info_3.creative_set_id = "5800049f-cee5-4bcb-90c7-85246d5f5e7c";
  info_3.campaign_id = "3d62eca2-324a-4161-a0c5-7d9f29d10ab0";
  info_3.start_at_timestamp = DistantPastAsTimestamp();
  info_3.end_at_timestamp = DistantFutureAsTimestamp();
  info_3.daily_cap = 1;
  info_3.advertiser_id = "9a11b60f-e29d-4446-8d1f-318311e36e0a";
  info_3.priority = 2;
  info_3.per_day = 3;
  info_3.total_max = 4;
  info_3.segment = "Technology & Computing-Software";
  info_3.dayparts.push_back(daypart_info);
  info_3.geo_targets = {"US"};
  info_3.target_url = "https://brave.com";
  info_3.title = "Test Ad 3 Title";
  info_3.description = "Test Ad 3 Body";
  info_3.ptr = 1.0;
  creative_brave_today_ads.push_back(info_3);

  // Act
  Save(creative_brave_today_ads);

  // Assert
  const CreativeBraveTodayAdList expected_creative_brave_today_ads =
      creative_brave_today_ads;

  const SegmentList segments = {"Technology & Computing-Software"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       DoNotSaveDuplicateCreativeBraveTodayAds) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPastAsTimestamp();
  info.end_at_timestamp = DistantFutureAsTimestamp();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.segment = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = {"US"};
  info.target_url = "https://brave.com";
  info.title = "Test Ad 1 Title";
  info.description = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_brave_today_ads.push_back(info);

  Save(creative_brave_today_ads);

  // Act
  Save(creative_brave_today_ads);

  // Assert
  const CreativeBraveTodayAdList expected_creative_brave_today_ads =
      creative_brave_today_ads;

  const SegmentList segments = {"Technology & Computing-Software"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest, GetForCategories) {
  // Arrange

  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPastAsTimestamp();
  info_1.end_at_timestamp = DistantFutureAsTimestamp();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.segment = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = {"US"};
  info_1.target_url = "https://brave.com";
  info_1.title = "Test Ad 1 Title";
  info_1.description = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_brave_today_ads.push_back(info_1);

  CreativeBraveTodayAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPastAsTimestamp();
  info_2.end_at_timestamp = DistantFutureAsTimestamp();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.segment = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = {"US"};
  info_2.target_url = "https://brave.com";
  info_2.title = "Test Ad 2 Title";
  info_2.description = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_brave_today_ads.push_back(info_2);

  Save(creative_brave_today_ads);

  // Act

  // Assert
  const CreativeBraveTodayAdList expected_creative_brave_today_ads =
      creative_brave_today_ads;

  const SegmentList segments = {"Technology & Computing-Software"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       GetCreativeBraveTodayAdsForCreativeInstanceId) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPastAsTimestamp();
  info.end_at_timestamp = DistantFutureAsTimestamp();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.segment = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = {"US"};
  info.target_url = "https://brave.com";
  info.title = "Test Ad 1 Title";
  info.description = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_brave_today_ads.push_back(info);

  Save(creative_brave_today_ads);

  // Act

  // Assert
  const CreativeBraveTodayAdInfo expected_creative_brave_today_ad = info;

  const std::string creative_instance_id =
      "3519f52c-46a4-4c48-9c2b-c264c0067f04";

  database_table_->GetForCreativeInstanceId(
      creative_instance_id,
      [&expected_creative_brave_today_ad](
          const Result result, const std::string& creative_instance_id,
          const CreativeBraveTodayAdInfo& creative_brave_today_ad) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_EQ(expected_creative_brave_today_ad, creative_brave_today_ad);
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       GetCreativeBraveTodayAdsForNonExistentCreativeInstanceId) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPastAsTimestamp();
  info.end_at_timestamp = DistantFutureAsTimestamp();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.segment = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = {"US"};
  info.target_url = "https://brave.com";
  info.title = "Test Ad 1 Title";
  info.description = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_brave_today_ads.push_back(info);

  Save(creative_brave_today_ads);

  // Act

  // Assert
  const std::string creative_instance_id =
      "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

  database_table_->GetForCreativeInstanceId(
      creative_instance_id,
      [](const Result result, const std::string& creative_instance_id,
         const CreativeBraveTodayAdInfo& creative_brave_today_ad) {
        EXPECT_EQ(Result::FAILED, result);
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       GetCreativeBraveTodayAdsForEmptyCategories) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPastAsTimestamp();
  info.end_at_timestamp = DistantFutureAsTimestamp();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.segment = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = {"US"};
  info.target_url = "https://brave.com";
  info.title = "Test Ad 1 Title";
  info.description = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_brave_today_ads.push_back(info);

  Save(creative_brave_today_ads);

  // Act

  // Assert
  const CreativeBraveTodayAdList expected_creative_brave_today_ads = {};

  const SegmentList segments = {};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       GetCreativeBraveTodayAdsForNonExistentCategory) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info;
  info.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info.start_at_timestamp = DistantPastAsTimestamp();
  info.end_at_timestamp = DistantFutureAsTimestamp();
  info.daily_cap = 1;
  info.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info.priority = 2;
  info.per_day = 3;
  info.total_max = 4;
  info.segment = "Technology & Computing-Software";
  info.dayparts.push_back(daypart_info);
  info.geo_targets = {"US"};
  info.target_url = "https://brave.com";
  info.title = "Test Ad 1 Title";
  info.description = "Test Ad 1 Body";
  info.ptr = 1.0;
  creative_brave_today_ads.push_back(info);

  Save(creative_brave_today_ads);

  // Act

  // Assert
  const CreativeBraveTodayAdList expected_creative_brave_today_ads = {};

  const SegmentList segments = {"Food & Drink"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       GetCreativeBraveTodayAdsFromMultipleCategories) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPastAsTimestamp();
  info_1.end_at_timestamp = DistantFutureAsTimestamp();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.segment = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = {"US"};
  info_1.target_url = "https://brave.com";
  info_1.title = "Test Ad 1 Title";
  info_1.description = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_brave_today_ads.push_back(info_1);

  CreativeBraveTodayAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPastAsTimestamp();
  info_2.end_at_timestamp = DistantFutureAsTimestamp();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.segment = "Food & Drink";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = {"US"};
  info_2.target_url = "https://brave.com";
  info_2.title = "Test Ad 2 Title";
  info_2.description = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_brave_today_ads.push_back(info_2);

  CreativeBraveTodayAdInfo info_3;
  info_3.creative_instance_id = "a1ac44c2-675f-43e6-ab6d-500614cafe63";
  info_3.creative_set_id = "5800049f-cee5-4bcb-90c7-85246d5f5e7c";
  info_3.campaign_id = "3d62eca2-324a-4161-a0c5-7d9f29d10ab0";
  info_3.start_at_timestamp = DistantPastAsTimestamp();
  info_3.end_at_timestamp = DistantFutureAsTimestamp();
  info_3.daily_cap = 1;
  info_3.advertiser_id = "9a11b60f-e29d-4446-8d1f-318311e36e0a";
  info_3.priority = 2;
  info_3.per_day = 3;
  info_3.total_max = 4;
  info_3.segment = "Automobiles";
  info_3.dayparts.push_back(daypart_info);
  info_3.geo_targets = {"US"};
  info_3.target_url = "https://brave.com";
  info_3.title = "Test Ad 3 Title";
  info_3.description = "Test Ad 3 Body";
  info_3.ptr = 1.0;
  creative_brave_today_ads.push_back(info_3);

  Save(creative_brave_today_ads);

  // Act

  // Assert
  CreativeBraveTodayAdList expected_creative_brave_today_ads;
  expected_creative_brave_today_ads.push_back(info_1);
  expected_creative_brave_today_ads.push_back(info_2);

  const SegmentList segments = {"Technology & Computing-Software",
                                "Food & Drink"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       GetNonExpiredCreativeBraveTodayAds) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPastAsTimestamp();
  info_1.end_at_timestamp = NowAsTimestamp();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.segment = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = {"US"};
  info_1.target_url = "https://brave.com";
  info_1.title = "Test Ad 1 Title";
  info_1.description = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_brave_today_ads.push_back(info_1);

  CreativeBraveTodayAdInfo info_2;
  info_2.creative_instance_id = "eaa6224a-876d-4ef8-a384-9ac34f238631";
  info_2.creative_set_id = "184d1fdd-8e18-4baa-909c-9a3cb62cc7b1";
  info_2.campaign_id = "d1d4a649-502d-4e06-b4b8-dae11c382d26";
  info_2.start_at_timestamp = DistantPastAsTimestamp();
  info_2.end_at_timestamp = DistantFutureAsTimestamp();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "8e3fac86-ce50-4409-ae29-9aa5636aa9a2";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.segment = "Technology & Computing-Software";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = {"US"};
  info_2.target_url = "https://brave.com";
  info_2.title = "Test Ad 2 Title";
  info_2.description = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_brave_today_ads.push_back(info_2);

  Save(creative_brave_today_ads);

  // Act
  FastForwardClockBy(base::TimeDelta::FromHours(1));

  // Assert
  CreativeBraveTodayAdList expected_creative_brave_today_ads;
  expected_creative_brave_today_ads.push_back(info_2);

  const SegmentList segments = {"Technology & Computing-Software"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest,
       GetCreativeBraveTodayAdsMatchingCaseInsensitiveCategories) {
  // Arrange
  CreativeBraveTodayAdList creative_brave_today_ads;

  CreativeDaypartInfo daypart_info;
  CreativeBraveTodayAdInfo info_1;
  info_1.creative_instance_id = "3519f52c-46a4-4c48-9c2b-c264c0067f04";
  info_1.creative_set_id = "c2ba3e7d-f688-4bc4-a053-cbe7ac1e6123";
  info_1.campaign_id = "84197fc8-830a-4a8e-8339-7a70c2bfa104";
  info_1.start_at_timestamp = DistantPastAsTimestamp();
  info_1.end_at_timestamp = DistantFutureAsTimestamp();
  info_1.daily_cap = 1;
  info_1.advertiser_id = "5484a63f-eb99-4ba5-a3b0-8c25d3c0e4b2";
  info_1.priority = 2;
  info_1.per_day = 3;
  info_1.total_max = 4;
  info_1.segment = "Technology & Computing-Software";
  info_1.dayparts.push_back(daypart_info);
  info_1.geo_targets = {"US"};
  info_1.target_url = "https://brave.com";
  info_1.title = "Test Ad 1 Title";
  info_1.description = "Test Ad 1 Body";
  info_1.ptr = 1.0;
  creative_brave_today_ads.push_back(info_1);

  CreativeBraveTodayAdInfo info_2;
  info_2.creative_instance_id = "a1ac44c2-675f-43e6-ab6d-500614cafe63";
  info_2.creative_set_id = "5800049f-cee5-4bcb-90c7-85246d5f5e7c";
  info_2.campaign_id = "3d62eca2-324a-4161-a0c5-7d9f29d10ab0";
  info_2.start_at_timestamp = DistantPastAsTimestamp();
  info_2.end_at_timestamp = DistantFutureAsTimestamp();
  info_2.daily_cap = 1;
  info_2.advertiser_id = "9a11b60f-e29d-4446-8d1f-318311e36e0a";
  info_2.priority = 2;
  info_2.per_day = 3;
  info_2.total_max = 4;
  info_2.segment = "Food & Drink";
  info_2.dayparts.push_back(daypart_info);
  info_2.geo_targets = {"US"};
  info_2.target_url = "https://brave.com";
  info_2.title = "Test Ad 2 Title";
  info_2.description = "Test Ad 2 Body";
  info_2.ptr = 1.0;
  creative_brave_today_ads.push_back(info_2);

  Save(creative_brave_today_ads);

  // Act

  // Assert
  CreativeBraveTodayAdList expected_creative_brave_today_ads;
  expected_creative_brave_today_ads.push_back(info_2);

  const SegmentList segments = {"FoOd & DrInK"};

  database_table_->GetForSegments(
      segments, [&expected_creative_brave_today_ads](
                    const Result result, const SegmentList& segments,
                    const CreativeBraveTodayAdList& creative_brave_today_ads) {
        EXPECT_EQ(Result::SUCCESS, result);
        EXPECT_TRUE(CompareAsSets(expected_creative_brave_today_ads,
                                  creative_brave_today_ads));
      });
}

TEST_F(BatAdsCreativeBraveTodayAdsDatabaseTableTest, TableName) {
  // Arrange

  // Act
  const std::string table_name = database_table_->get_table_name();

  // Assert
  const std::string expected_table_name = "creative_brave_today_ads";
  EXPECT_EQ(expected_table_name, table_name);
}

}  // namespace ads
