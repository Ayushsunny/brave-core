/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_TX_CONTROLLER_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_TX_CONTROLLER_H_

#include <memory>

#include "base/memory/weak_ptr.h"
#include "brave/components/brave_wallet/browser/eth_nonce_tracker.h"
#include "brave/components/brave_wallet/browser/eth_tx_state_manager.h"

class BraveWalletService;

namespace brave_wallet {

class EthTxController {
 public:
  explicit EthTxController(base::WeakPtr<BraveWalletService> wallet_service);
  ~EthTxController();
  EthTxController(const EthTxController&) = delete;
  EthTxController operator=(const EthTxController&) = delete;

 private:
  base::WeakPtr<BraveWalletService> wallet_service_;
  std::unique_ptr<EthTxStateManager> tx_state_manager_;
  std::unique_ptr<EthNonceTracker> nonce_tracker_;

  base::WeakPtrFactory<EthTxController> weak_factory_;
};

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_TX_CONTROLLER_H_
