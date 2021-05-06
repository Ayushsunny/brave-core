/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "net/http/transport_security_state.h"

#include <algorithm>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "base/base64.h"
#include "base/bind.h"
#include "base/build_time.h"
#include "base/containers/span.h"
#include "base/feature_list.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/field_trial_params.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/optional.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/time/time_to_iso8601.h"
#include "base/values.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "crypto/sha2.h"
#include "net/base/features.h"
#include "net/base/hash_value.h"
#include "net/base/host_port_pair.h"
#include "net/cert/ct_policy_status.h"
#include "net/cert/symantec_certs.h"
#include "net/cert/x509_cert_types.h"
#include "net/cert/x509_certificate.h"
#include "net/dns/dns_util.h"
#include "net/extras/preload_data/decoder.h"
#include "net/http/hsts_info.h"
#include "net/http/http_security_headers.h"
#include "net/net_buildflags.h"
#include "net/ssl/ssl_info.h"

#undef BUILDFLAG_INTERNAL_GOOGLE_CHROME_BRANDING
// Force static pins to be enabled
#define BUILDFLAG_INTERNAL_GOOGLE_CHROME_BRANDING() (1)

#include "../../../../net/http/transport_security_state.cc"
