Energi Core version 2.1.0 Release Notes
=======================================

Energi Core version 2.1.0 is now available from:

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

**NOTE**: version of Energi Core prior to v2.0.0 must not be used with the same data files.


v2.1.0 changelog
----------------

Major changes:

* CHANGED: adjusted PoS consensus logic
* CHANGED: testnet reset to block 46999
* FIXED: to ignore best header from invalid ancestor on startup
  - Common Bitcoin/Dash issue
* NEW: parallel initial block download (at least x8 speedup)
* NEW: reset of invalid status of the correct chain on checkpoint failure
* NEW: checkpoint to the last superblock at 383040 & block 383300
* NEW: dynamic checkpoints support via spork functionality

Minor changes:

* CHANGED: updated checkpoints & minwork
* CHANGED: "Sync Headers" to "Initial sync" messages
* FIXED: PoS-enabled chain startup
* FIXED: ExecuteSpork() to be called on local node
* FIXED: active PoS detection on startup & ignore of invalid spork override
* FIXED: initial header sync blocking network stack for too long
* FIXED: placeholder sync text when block time is ahead of local time
* FIXED: default Spork 9 value
* FIXED: checkpoints to correctly handle edge cases with multiple forks
* FIXED: spork 12 limit to match Energi 24h block count
* NEW: chaintps estimations in getblock- RPC
