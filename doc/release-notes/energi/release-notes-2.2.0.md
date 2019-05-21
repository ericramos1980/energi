Energi Core version 2.2.0 Release Notes
=======================================

This is NOT a mandatory upgrade. Energi Core v2.1.x releases remain
acceptable for production use.

Energi Core version 2.2.0 is now available from:

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


v2.2.0 changelog
----------------

Major changes:

* NEW: scam alert for dump* family of debug console commands in UI mode
* NEW: "_this_is_your_wallet.txt" gets appended to dumpwallet in UI mode
* NEW: boostrap.dat generation solution
* NEW: whole chain UTXO snapshot feature for Energi 3.0 migration
* CHANGED: removed all themes except for "light"

Minor changes:

* CHANGED: updated dump* family of command scam warning wording
* CHANGED: update Energi logo & other graphics
* FIXED: weird block verification progress under some conditions
