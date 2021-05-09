/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/brave_today_ad_info.h"

#include "bat/ads/internal/json_helper.h"
#include "bat/ads/internal/logging.h"

namespace ads {

struct ConfirmationType;

BraveTodayAdInfo::BraveTodayAdInfo() = default;

BraveTodayAdInfo::BraveTodayAdInfo(const BraveTodayAdInfo& info) = default;

BraveTodayAdInfo::~BraveTodayAdInfo() = default;

bool BraveTodayAdInfo::IsValid() const {
  if (!AdInfo::IsValid()) {
    return false;
  }

  if (title.empty() || description.empty()) {
    return false;
  }

  return true;
}

std::string BraveTodayAdInfo::ToJson() const {
  std::string json;
  SaveToJson(*this, &json);
  return json;
}

Result BraveTodayAdInfo::FromJson(const std::string& json) {
  rapidjson::Document document;
  document.Parse(json.c_str());

  if (document.HasParseError()) {
    BLOG(1, helper::JSON::GetLastError(&document));
    return FAILED;
  }

  if (document.HasMember("type")) {
    type = AdType(document["type"].GetString());
  }

  if (document.HasMember("uuid")) {
    uuid = document["uuid"].GetString();
  }

  if (document.HasMember("creative_instance_id")) {
    creative_instance_id = document["creative_instance_id"].GetString();
  }

  if (document.HasMember("creative_set_id")) {
    creative_set_id = document["creative_set_id"].GetString();
  }

  if (document.HasMember("campaign_id")) {
    campaign_id = document["campaign_id"].GetString();
  }

  if (document.HasMember("advertiser_id")) {
    advertiser_id = document["advertiser_id"].GetString();
  }

  if (document.HasMember("segment")) {
    segment = document["segment"].GetString();
  }

  if (document.HasMember("title")) {
    title = document["title"].GetString();
  }

  if (document.HasMember("description")) {
    description = document["description"].GetString();
  }

  if (document.HasMember("image_url")) {
    image_url = document["image_url"].GetString();
  }

  if (document.HasMember("size")) {
    size = document["size"].GetString();
  }

  if (document.HasMember("target_url")) {
    target_url = document["target_url"].GetString();
  }

  return SUCCESS;
}

void SaveToJson(JsonWriter* writer, const BraveTodayAdInfo& info) {
  writer->StartObject();

  writer->String("type");
  const std::string type = std::string(info.type);
  writer->String(type.c_str());

  writer->String("uuid");
  writer->String(info.uuid.c_str());

  writer->String("creative_instance_id");
  writer->String(info.creative_instance_id.c_str());

  writer->String("creative_set_id");
  writer->String(info.creative_set_id.c_str());

  writer->String("campaign_id");
  writer->String(info.campaign_id.c_str());

  writer->String("advertiser_id");
  writer->String(info.advertiser_id.c_str());

  writer->String("segment");
  writer->String(info.segment.c_str());

  writer->String("title");
  writer->String(info.title.c_str());

  writer->String("description");
  writer->String(info.description.c_str());

  writer->String("image_url");
  writer->String(info.image_url.c_str());

  writer->String("size");
  writer->String(info.size.c_str());

  writer->String("target_url");
  writer->String(info.target_url.c_str());

  writer->EndObject();
}

}  // namespace ads
