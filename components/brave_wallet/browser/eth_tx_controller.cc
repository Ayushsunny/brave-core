/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eth_tx_controller.h"

#include "brave/components/brave_wallet/browser/brave_wallet_service.h"
#include "brave/components/brave_wallet/browser/eth_address.h"

namespace brave_wallet {

EthTxController::EthTxController(
    base::WeakPtr<BraveWalletService> wallet_service)
    : wallet_service_(wallet_service),
      tx_state_manager_(std::make_unique<EthTxStateManager>()),
      nonce_tracker_(
          std::make_unique<EthNonceTracker>(tx_state_manager_.get(),
                                            wallet_service_->controller())),
      weak_factory_(this) {}
EthTxController::~EthTxController() = default;

}  // namespace brave_wallet
