Energi Core version 2.3.0 Release Notes
=======================================

This is NOT a mandatory upgrade, but it is HIGHLY recommended.

Energi Core version 2.3.0 is now available from:

  https://energi.world/downloads

Please report bugs using the issue tracker at github:

  https://github.com/energicryptocurrency/energi/issues


How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then depending on
your operation system:

* for Window run the installer
* for macos run the installer
* for Linux unpack to a separate folder and run from there

**NOTE**: version of Energi Core prior to v2.1.0 must not be used


v2.3.0 changelog
----------------

Major changes:

* NEW: SPORK17 - PoS v1.3 block time soft fork
  - prevents blocks in the future
  - enforces block time to be after it parent
  - older by block time takes precedence instead of older by receive time
  - new bit 3 is used to indicate miner readiness
* CHANGED: time adjustment according to network is disabled in favor of pure NTP sync
* CHANGED: coin selection is capped to 10K UTXOs
  - this may affect RPC calls and transaction sending
* CHANGED: PrivateSend is disabled in clients in preparation for Gen 3 migration
* FIXED: UI performance issues with large UTXO wallets
  - UTXO count is capped to 100K items for display/update purposes

Minor changes:

* CHANGED: protocol version bumped to 7.2.13 without actual message changes
* CHANGED: extended migration snapshot format
* NEW: miner strategy support for SPORK 17
* NEW: updated checkpoints
