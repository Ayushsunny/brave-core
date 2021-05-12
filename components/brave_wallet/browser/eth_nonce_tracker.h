/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_NONCE_TRACKER_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_NONCE_TRACKER_H_

#include <string>

#include "base/containers/flat_map.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/brave_wallet/browser/brave_wallet_types.h"

namespace brave_wallet {

class EthAddress;
class EthJsonRpcController;
class EthTxStateManager;

class EthNonceTracker {
 public:
  EthNonceTracker(EthTxStateManager* tx_state_manager,
                  EthJsonRpcController* rpc_controller);
  ~EthNonceTracker();
  EthNonceTracker(const EthNonceTracker&) = delete;
  EthNonceTracker operator=(const EthNonceTracker&) = delete;

  bool GetNextNonce(const EthAddress& from, uint256_t* nonce);

 private:
  void OnGetNetworkNonce(std::string address, bool status, uint256_t result);

  EthTxStateManager* tx_state_manager_;
  EthJsonRpcController* rpc_controller_;

  // (address, nonce)
  base::flat_map<std::string, uint256_t> network_nonce_map_;

  base::WeakPtrFactory<EthNonceTracker> weak_factory_;
};

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_NONCE_TRACKER_H_
