/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/database/tables/creative_brave_today_ads_database_table.h"

#include <algorithm>
#include <utility>

#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/time/time.h"
#include "bat/ads/internal/ads_client_helper.h"
#include "bat/ads/internal/container_util.h"
#include "bat/ads/internal/database/database_statement_util.h"
#include "bat/ads/internal/database/database_table_util.h"
#include "bat/ads/internal/database/database_util.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/time_formatting_util.h"

namespace ads {
namespace database {
namespace table {

namespace {

const char kTableName[] = "creative_brave_today_ads";

const int kDefaultBatchSize = 50;

}  // namespace

CreativeBraveTodayAds::CreativeBraveTodayAds()
    : batch_size_(kDefaultBatchSize),
      campaigns_database_table_(std::make_unique<Campaigns>()),
      creative_ads_database_table_(std::make_unique<CreativeAds>()),
      dayparts_database_table_(std::make_unique<Dayparts>()),
      geo_targets_database_table_(std::make_unique<GeoTargets>()),
      segments_database_table_(std::make_unique<Segments>()) {}

CreativeBraveTodayAds::~CreativeBraveTodayAds() = default;

void CreativeBraveTodayAds::Save(
    const CreativeBraveTodayAdList& creative_brave_today_ads,
    ResultCallback callback) {
  if (creative_brave_today_ads.empty()) {
    callback(Result::SUCCESS);
    return;
  }

  DBTransactionPtr transaction = DBTransaction::New();

  const std::vector<CreativeBraveTodayAdList> batches =
      SplitVector(creative_brave_today_ads, batch_size_);

  for (const auto& batch : batches) {
    InsertOrUpdate(transaction.get(), batch);

    std::vector<CreativeAdInfo> creative_ads(batch.begin(), batch.end());
    campaigns_database_table_->InsertOrUpdate(transaction.get(), creative_ads);
    creative_ads_database_table_->InsertOrUpdate(transaction.get(),
                                                 creative_ads);
    dayparts_database_table_->InsertOrUpdate(transaction.get(), creative_ads);
    geo_targets_database_table_->InsertOrUpdate(transaction.get(),
                                                creative_ads);
    segments_database_table_->InsertOrUpdate(transaction.get(), creative_ads);
  }

  AdsClientHelper::Get()->RunDBTransaction(
      std::move(transaction),
      std::bind(&OnResultCallback, std::placeholders::_1, callback));
}

void CreativeBraveTodayAds::Delete(ResultCallback callback) {
  DBTransactionPtr transaction = DBTransaction::New();

  util::Delete(transaction.get(), get_table_name());

  AdsClientHelper::Get()->RunDBTransaction(
      std::move(transaction),
      std::bind(&OnResultCallback, std::placeholders::_1, callback));
}

void CreativeBraveTodayAds::GetForCreativeInstanceId(
    const std::string& creative_instance_id,
    GetCreativeBraveTodayAdCallback callback) {
  CreativeBraveTodayAdInfo creative_brave_today_ad;

  if (creative_instance_id.empty()) {
    callback(Result::FAILED, creative_instance_id, creative_brave_today_ad);
    return;
  }

  const std::string query = base::StringPrintf(
      "SELECT "
      "cpca.creative_instance_id, "
      "cpca.creative_set_id, "
      "cpca.campaign_id, "
      "cam.start_at_timestamp, "
      "cam.end_at_timestamp, "
      "cam.daily_cap, "
      "cam.advertiser_id, "
      "cam.priority, "
      "ca.conversion, "
      "ca.per_day, "
      "ca.per_week, "
      "ca.per_month, "
      "ca.total_max, "
      "ca.split_test_group, "
      "s.segment, "
      "gt.geo_target, "
      "ca.target_url, "
      "cpca.title, "
      "cpca.description, "
      "cam.ptr, "
      "dp.dow, "
      "dp.start_minute, "
      "dp.end_minute "
      "FROM %s AS cpca "
      "INNER JOIN campaigns AS cam "
      "ON cam.campaign_id = cpca.campaign_id "
      "INNER JOIN segments AS s "
      "ON s.creative_set_id = cpca.creative_set_id "
      "INNER JOIN creative_ads AS ca "
      "ON ca.creative_instance_id = cpca.creative_instance_id "
      "INNER JOIN geo_targets AS gt "
      "ON gt.campaign_id = cpca.campaign_id "
      "INNER JOIN dayparts AS dp "
      "ON dp.campaign_id = cpca.campaign_id "
      "WHERE cpca.creative_instance_id = '%s'",
      get_table_name().c_str(), creative_instance_id.c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::READ;
  command->command = query;

  command->record_bindings = {
      DBCommand::RecordBindingType::STRING_TYPE,  // creative_instance_id
      DBCommand::RecordBindingType::STRING_TYPE,  // creative_set_id
      DBCommand::RecordBindingType::STRING_TYPE,  // campaign_id
      DBCommand::RecordBindingType::INT64_TYPE,   // start_at_timestamp
      DBCommand::RecordBindingType::INT64_TYPE,   // end_at_timestamp
      DBCommand::RecordBindingType::INT_TYPE,     // daily_cap
      DBCommand::RecordBindingType::STRING_TYPE,  // advertiser_id
      DBCommand::RecordBindingType::INT_TYPE,     // priority
      DBCommand::RecordBindingType::BOOL_TYPE,    // conversion
      DBCommand::RecordBindingType::INT_TYPE,     // per_day
      DBCommand::RecordBindingType::INT_TYPE,     // per_week
      DBCommand::RecordBindingType::INT_TYPE,     // per_month
      DBCommand::RecordBindingType::INT_TYPE,     // total_max
      DBCommand::RecordBindingType::STRING_TYPE,  // split_test_group
      DBCommand::RecordBindingType::STRING_TYPE,  // segment
      DBCommand::RecordBindingType::STRING_TYPE,  // geo_target
      DBCommand::RecordBindingType::STRING_TYPE,  // target_url
      DBCommand::RecordBindingType::STRING_TYPE,  // title
      DBCommand::RecordBindingType::STRING_TYPE,  // description
      DBCommand::RecordBindingType::DOUBLE_TYPE,  // ptr
      DBCommand::RecordBindingType::STRING_TYPE,  // dayparts->dow
      DBCommand::RecordBindingType::INT_TYPE,     // dayparts->start_minute
      DBCommand::RecordBindingType::INT_TYPE      // dayparts->end_minute
  };

  DBTransactionPtr transaction = DBTransaction::New();
  transaction->commands.push_back(std::move(command));

  AdsClientHelper::Get()->RunDBTransaction(
      std::move(transaction),
      std::bind(&CreativeBraveTodayAds::OnGetForCreativeInstanceId, this,
                std::placeholders::_1, creative_instance_id, callback));
}

void CreativeBraveTodayAds::GetForSegments(
    const SegmentList& segments,
    GetCreativeBraveTodayAdsCallback callback) {
  if (segments.empty()) {
    callback(Result::SUCCESS, segments, {});
    return;
  }

  const std::string query = base::StringPrintf(
      "SELECT "
      "cpca.creative_instance_id, "
      "cpca.creative_set_id, "
      "cpca.campaign_id, "
      "cam.start_at_timestamp, "
      "cam.end_at_timestamp, "
      "cam.daily_cap, "
      "cam.advertiser_id, "
      "cam.priority, "
      "ca.conversion, "
      "ca.per_day, "
      "ca.per_week, "
      "ca.per_month, "
      "ca.total_max, "
      "ca.split_test_group, "
      "s.segment, "
      "gt.geo_target, "
      "ca.target_url, "
      "cpca.title, "
      "cpca.description, "
      "cam.ptr, "
      "dp.dow, "
      "dp.start_minute, "
      "dp.end_minute "
      "FROM %s AS cpca "
      "INNER JOIN campaigns AS cam "
      "ON cam.campaign_id = cpca.campaign_id "
      "INNER JOIN segments AS s "
      "ON s.creative_set_id = cpca.creative_set_id "
      "INNER JOIN creative_ads AS ca "
      "ON ca.creative_instance_id = cpca.creative_instance_id "
      "INNER JOIN geo_targets AS gt "
      "ON gt.campaign_id = cpca.campaign_id "
      "INNER JOIN dayparts AS dp "
      "ON dp.campaign_id = cpca.campaign_id "
      "WHERE s.segment IN %s "
      "AND %s BETWEEN cam.start_at_timestamp AND cam.end_at_timestamp",
      get_table_name().c_str(),
      BuildBindingParameterPlaceholder(segments.size()).c_str(),
      TimeAsTimestampString(base::Time::Now()).c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::READ;
  command->command = query;

  int index = 0;
  for (const auto& segment : segments) {
    BindString(command.get(), index, base::ToLowerASCII(segment));
    index++;
  }

  command->record_bindings = {
      DBCommand::RecordBindingType::STRING_TYPE,  // creative_instance_id
      DBCommand::RecordBindingType::STRING_TYPE,  // creative_set_id
      DBCommand::RecordBindingType::STRING_TYPE,  // campaign_id
      DBCommand::RecordBindingType::INT64_TYPE,   // start_at_timestamp
      DBCommand::RecordBindingType::INT64_TYPE,   // end_at_timestamp
      DBCommand::RecordBindingType::INT_TYPE,     // daily_cap
      DBCommand::RecordBindingType::STRING_TYPE,  // advertiser_id
      DBCommand::RecordBindingType::INT_TYPE,     // priority
      DBCommand::RecordBindingType::BOOL_TYPE,    // conversion
      DBCommand::RecordBindingType::INT_TYPE,     // per_day
      DBCommand::RecordBindingType::INT_TYPE,     // per_week
      DBCommand::RecordBindingType::INT_TYPE,     // per_month
      DBCommand::RecordBindingType::INT_TYPE,     // total_max
      DBCommand::RecordBindingType::STRING_TYPE,  // split_test_group
      DBCommand::RecordBindingType::STRING_TYPE,  // segment
      DBCommand::RecordBindingType::STRING_TYPE,  // geo_target
      DBCommand::RecordBindingType::STRING_TYPE,  // target_url
      DBCommand::RecordBindingType::STRING_TYPE,  // title
      DBCommand::RecordBindingType::STRING_TYPE,  // description
      DBCommand::RecordBindingType::DOUBLE_TYPE,  // ptr
      DBCommand::RecordBindingType::STRING_TYPE,  // dayparts->dow
      DBCommand::RecordBindingType::INT_TYPE,     // dayparts->start_minute
      DBCommand::RecordBindingType::INT_TYPE      // dayparts->end_minute
  };

  DBTransactionPtr transaction = DBTransaction::New();
  transaction->commands.push_back(std::move(command));

  AdsClientHelper::Get()->RunDBTransaction(
      std::move(transaction),
      std::bind(&CreativeBraveTodayAds::OnGetForSegments, this,
                std::placeholders::_1, segments, callback));
}

void CreativeBraveTodayAds::GetAll(GetCreativeBraveTodayAdsCallback callback) {
  const std::string query = base::StringPrintf(
      "SELECT "
      "cpca.creative_instance_id, "
      "cpca.creative_set_id, "
      "cpca.campaign_id, "
      "cam.start_at_timestamp, "
      "cam.end_at_timestamp, "
      "cam.daily_cap, "
      "cam.advertiser_id, "
      "cam.priority, "
      "ca.conversion, "
      "ca.per_day, "
      "ca.per_week, "
      "ca.per_month, "
      "ca.total_max, "
      "ca.split_test_group, "
      "s.segment, "
      "gt.geo_target, "
      "ca.target_url, "
      "cpca.title, "
      "cpca.description, "
      "cam.ptr, "
      "dp.dow, "
      "dp.start_minute, "
      "dp.end_minute "
      "FROM %s AS cpca "
      "INNER JOIN campaigns AS cam "
      "ON cam.campaign_id = cpca.campaign_id "
      "INNER JOIN segments AS s "
      "ON s.creative_set_id = cpca.creative_set_id "
      "INNER JOIN creative_ads AS ca "
      "ON ca.creative_instance_id = cpca.creative_instance_id "
      "INNER JOIN geo_targets AS gt "
      "ON gt.campaign_id = cpca.campaign_id "
      "INNER JOIN dayparts AS dp "
      "ON dp.campaign_id = cpca.campaign_id "
      "WHERE %s BETWEEN cam.start_at_timestamp AND cam.end_at_timestamp",
      get_table_name().c_str(),
      TimeAsTimestampString(base::Time::Now()).c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::READ;
  command->command = query;

  command->record_bindings = {
      DBCommand::RecordBindingType::STRING_TYPE,  // creative_instance_id
      DBCommand::RecordBindingType::STRING_TYPE,  // creative_set_id
      DBCommand::RecordBindingType::STRING_TYPE,  // campaign_id
      DBCommand::RecordBindingType::INT64_TYPE,   // start_at_timestamp
      DBCommand::RecordBindingType::INT64_TYPE,   // end_at_timestamp
      DBCommand::RecordBindingType::INT_TYPE,     // daily_cap
      DBCommand::RecordBindingType::STRING_TYPE,  // advertiser_id
      DBCommand::RecordBindingType::INT_TYPE,     // priority
      DBCommand::RecordBindingType::BOOL_TYPE,    // conversion
      DBCommand::RecordBindingType::INT_TYPE,     // per_day
      DBCommand::RecordBindingType::INT_TYPE,     // per_week
      DBCommand::RecordBindingType::INT_TYPE,     // per_month
      DBCommand::RecordBindingType::INT_TYPE,     // total_max
      DBCommand::RecordBindingType::STRING_TYPE,  // split_test_group
      DBCommand::RecordBindingType::STRING_TYPE,  // segment
      DBCommand::RecordBindingType::STRING_TYPE,  // geo_target
      DBCommand::RecordBindingType::STRING_TYPE,  // target_url
      DBCommand::RecordBindingType::STRING_TYPE,  // title
      DBCommand::RecordBindingType::STRING_TYPE,  // description
      DBCommand::RecordBindingType::DOUBLE_TYPE,  // ptr
      DBCommand::RecordBindingType::STRING_TYPE,  // dayparts->dow
      DBCommand::RecordBindingType::INT_TYPE,     // dayparts->start_minute
      DBCommand::RecordBindingType::INT_TYPE      // dayparts->end_minute
  };

  DBTransactionPtr transaction = DBTransaction::New();
  transaction->commands.push_back(std::move(command));

  AdsClientHelper::Get()->RunDBTransaction(
      std::move(transaction), std::bind(&CreativeBraveTodayAds::OnGetAll, this,
                                        std::placeholders::_1, callback));
}

void CreativeBraveTodayAds::set_batch_size(const int batch_size) {
  DCHECK_GT(batch_size, 0);

  batch_size_ = batch_size;
}

std::string CreativeBraveTodayAds::get_table_name() const {
  return kTableName;
}

void CreativeBraveTodayAds::Migrate(DBTransaction* transaction,
                                    const int to_version) {
  DCHECK(transaction);

  switch (to_version) {
    case 14: {
      MigrateToV14(transaction);
      break;
    }

    default: {
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

void CreativeBraveTodayAds::InsertOrUpdate(
    DBTransaction* transaction,
    const CreativeBraveTodayAdList& creative_brave_today_ads) {
  DCHECK(transaction);

  if (creative_brave_today_ads.empty()) {
    return;
  }

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::RUN;
  command->command =
      BuildInsertOrUpdateQuery(command.get(), creative_brave_today_ads);

  transaction->commands.push_back(std::move(command));
}

int CreativeBraveTodayAds::BindParameters(
    DBCommand* command,
    const CreativeBraveTodayAdList& creative_brave_today_ads) {
  DCHECK(command);

  int count = 0;

  int index = 0;
  for (const auto& creative_brave_today_ad : creative_brave_today_ads) {
    BindString(command, index++, creative_brave_today_ad.creative_instance_id);
    BindString(command, index++, creative_brave_today_ad.creative_set_id);
    BindString(command, index++, creative_brave_today_ad.campaign_id);
    BindString(command, index++, creative_brave_today_ad.title);
    BindString(command, index++, creative_brave_today_ad.description);

    count++;
  }

  return count;
}

std::string CreativeBraveTodayAds::BuildInsertOrUpdateQuery(
    DBCommand* command,
    const CreativeBraveTodayAdList& creative_brave_today_ads) {
  const int count = BindParameters(command, creative_brave_today_ads);

  return base::StringPrintf(
      "INSERT OR REPLACE INTO %s "
      "(creative_instance_id, "
      "creative_set_id, "
      "campaign_id, "
      "title, "
      "description) VALUES %s",
      get_table_name().c_str(),
      BuildBindingParameterPlaceholders(5, count).c_str());
}

void CreativeBraveTodayAds::OnGetForCreativeInstanceId(
    DBCommandResponsePtr response,
    const std::string& creative_instance_id,
    GetCreativeBraveTodayAdCallback callback) {
  if (!response || response->status != DBCommandResponse::Status::RESPONSE_OK) {
    BLOG(0, "Failed to get creative Brave Today ad");
    callback(Result::FAILED, creative_instance_id, {});
    return;
  }

  if (response->result->get_records().size() != 1) {
    BLOG(0, "Failed to get creative new tab page ad");
    callback(Result::FAILED, creative_instance_id, {});
    return;
  }

  DBRecord* record = response->result->get_records().at(0).get();

  const CreativeBraveTodayAdInfo creative_brave_today_ad =
      GetFromRecord(record);

  callback(Result::SUCCESS, creative_instance_id, creative_brave_today_ad);
}

void CreativeBraveTodayAds::OnGetForSegments(
    DBCommandResponsePtr response,
    const SegmentList& segments,
    GetCreativeBraveTodayAdsCallback callback) {
  if (!response || response->status != DBCommandResponse::Status::RESPONSE_OK) {
    BLOG(0, "Failed to get creative Brave Today ads");
    callback(Result::FAILED, segments, {});
    return;
  }

  CreativeBraveTodayAdList creative_brave_today_ads;

  for (const auto& record : response->result->get_records()) {
    const CreativeBraveTodayAdInfo creative_brave_today_ad =
        GetFromRecord(record.get());

    creative_brave_today_ads.push_back(creative_brave_today_ad);
  }

  callback(Result::SUCCESS, segments, creative_brave_today_ads);
}

void CreativeBraveTodayAds::OnGetAll(
    DBCommandResponsePtr response,
    GetCreativeBraveTodayAdsCallback callback) {
  if (!response || response->status != DBCommandResponse::Status::RESPONSE_OK) {
    BLOG(0, "Failed to get all creative Brave Today ads");
    callback(Result::FAILED, {}, {});
    return;
  }

  CreativeBraveTodayAdList creative_brave_today_ads;

  SegmentList segments;

  for (const auto& record : response->result->get_records()) {
    const CreativeBraveTodayAdInfo creative_brave_today_ad =
        GetFromRecord(record.get());

    creative_brave_today_ads.push_back(creative_brave_today_ad);

    segments.push_back(creative_brave_today_ad.segment);
  }

  std::sort(segments.begin(), segments.end());
  const auto iter = std::unique(segments.begin(), segments.end());
  segments.erase(iter, segments.end());

  callback(Result::SUCCESS, segments, creative_brave_today_ads);
}

CreativeBraveTodayAdInfo CreativeBraveTodayAds::GetFromRecord(
    DBRecord* record) const {
  CreativeBraveTodayAdInfo creative_brave_today_ad;

  creative_brave_today_ad.creative_instance_id = ColumnString(record, 0);
  creative_brave_today_ad.creative_set_id = ColumnString(record, 1);
  creative_brave_today_ad.campaign_id = ColumnString(record, 2);
  creative_brave_today_ad.start_at_timestamp = ColumnInt64(record, 3);
  creative_brave_today_ad.end_at_timestamp = ColumnInt64(record, 4);
  creative_brave_today_ad.daily_cap = ColumnInt(record, 5);
  creative_brave_today_ad.advertiser_id = ColumnString(record, 6);
  creative_brave_today_ad.priority = ColumnInt(record, 7);
  creative_brave_today_ad.conversion = ColumnBool(record, 8);
  creative_brave_today_ad.per_day = ColumnInt(record, 9);
  creative_brave_today_ad.per_week = ColumnInt(record, 10);
  creative_brave_today_ad.per_month = ColumnInt(record, 11);
  creative_brave_today_ad.total_max = ColumnInt(record, 12);
  creative_brave_today_ad.split_test_group = ColumnString(record, 13);
  creative_brave_today_ad.segment = ColumnString(record, 14);
  creative_brave_today_ad.geo_targets.push_back(ColumnString(record, 15));
  creative_brave_today_ad.target_url = ColumnString(record, 16);
  creative_brave_today_ad.title = ColumnString(record, 17);
  creative_brave_today_ad.description = ColumnString(record, 18);
  creative_brave_today_ad.ptr = ColumnDouble(record, 19);

  CreativeDaypartInfo daypart;
  daypart.dow = ColumnString(record, 20);
  daypart.start_minute = ColumnInt(record, 21);
  daypart.end_minute = ColumnInt(record, 22);
  creative_brave_today_ad.dayparts.push_back(daypart);

  return creative_brave_today_ad;
}

void CreativeBraveTodayAds::CreateTableV14(DBTransaction* transaction) {
  DCHECK(transaction);

  const std::string query = base::StringPrintf(
      "CREATE TABLE %s "
      "(creative_instance_id TEXT NOT NULL PRIMARY KEY UNIQUE "
      "ON CONFLICT REPLACE, "
      "creative_set_id TEXT NOT NULL, "
      "campaign_id TEXT NOT NULL, "
      "title TEXT NOT NULL, "
      "description TEXT NOT NULL)",
      get_table_name().c_str());

  DBCommandPtr command = DBCommand::New();
  command->type = DBCommand::Type::EXECUTE;
  command->command = query;

  transaction->commands.push_back(std::move(command));
}

void CreativeBraveTodayAds::MigrateToV14(DBTransaction* transaction) {
  DCHECK(transaction);

  util::Drop(transaction, get_table_name());

  CreateTableV14(transaction);
}

}  // namespace table
}  // namespace database
}  // namespace ads
