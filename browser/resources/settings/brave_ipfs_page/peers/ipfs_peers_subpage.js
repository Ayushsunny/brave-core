/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
import {Router} from '../router.js';
import {BraveIPFSBrowserProxyImpl} from './brave_ipfs_browser_proxy.m.js';

(function() {
  'use strict';
  
/**
* @fileoverview
* 'settings-sync-subpage' is the settings page content
*/
Polymer({
  is: 'settings-ipfs-peers-subpage',

  behaviors: [
    I18nBehavior,
    WebUIListenerBehavior
  ],

  properties: {
    /**
     * Array of sites to display in the widget.
     * @type {!Array<SiteException>}
     */
    keys: {
      type: Array,
      value() {
        return [];
      },
    },
    localNodeMethod: {
      type: Boolean,
      value: false,
      reflectToAttribute: true
    },
    localNodeLaunched: {
      type: Boolean,
      value: false
    },
    launchNodeButtonEnabled_: {
      type: Boolean,
      value: true,
    },
    localNodeLaunchError_: {
      type: Boolean,
      value: false,
    },
    importKeysError_: {
      type: Boolean,
      value: false,
    },
    showAddPeerDialog_: {
      type: Boolean,
      value: false,
    }
  },

  browserProxy_: null,

  /** @override */
  created: function() {
    this.browserProxy_ = BraveIPFSBrowserProxyImpl.getInstance();
    this.addWebUIListener('brave-ipfs-node-status-changed', (launched) => {
      this.onServiceLaunched(launched)
    })
  },

  toggleUILayout: function(launched) {
    this.launchNodeButtonEnabled_ = !launched;
    this.localNodeLaunched = launched
    this.importKeysError_ = false
    if (launched) {
      this.localNodeLaunchError_ = false
    } else {
      this.showAddPeerDialog_ = false
    }
  },

  onServiceLaunched: function(success) {
    this.toggleUILayout(success)
  },

  onStartNodeKeyTap_: function() {
    this.launchNodeButtonEnabled_ = false;
    this.browserProxy_.launchIPFSService().then((success) => {
      this.localNodeLaunchError_ = !success;
      this.onServiceLaunched(success)
    });
  },

  /*++++++
  * @override */
  ready: function() {
    this.onServiceLaunched(this.localNodeLaunched)
    window.addEventListener('load', this.onLoad_.bind(this));
  },

  onLoad_: function() {
    this.updatePeers();
  },

  isDefaultKey_: function(name) {
    return name == 'self';
  },

  onAddKeyTap_: function(item) {
    this.showAddPeerDialog_ = true
  },

  updatePeers: function() {
    this.browserProxy_.getIpnsKeysList().then(keys => {
      if (!keys)
        return;
      this.keys_ = JSON.parse(keys);
      this.toggleUILayout(true)
    });
  },

  onAddPeerDialogClosed_: function() {
    this.showAddPeerDialog_ = false
    this.updatePeers();
  },

  onPeerDeleteTapped_: function(event) {
    let name_to_remove = event.model.item.name
    var message = this.i18n('ipfsDeletePeerConfirmation', name_to_remove)
    if (!window.confirm(message))
      return
    this.browserProxy_.removeIpnsKey(name_to_remove).then(removed_name => {
      if (!removed_name)
        return;
      if (removed_name === name_to_remove) {
        this.updatePeers()
        return;
      }
    });
  }
});
})();
