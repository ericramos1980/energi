DEPRECATED
==========

This manual is deprecated, please see consistent development environment
instructions in [energi-docs/devenv.md](../energi-docs/devenv.md).

Mac OS X Build Instructions and Notes
====================================
This guide will show you how to build the Energi Core wallet for MacOS.
The built-in one is located in `/Applications/Utilities/Terminal.app`.
* Tested on OS X 10.7 through 10.13 on 64-bit Intel processors only.

Preparation
-----------
Install the OS X command line tools:

`xcode-select --install`

When the popup appears, click `Install`.

Then install [Homebrew](http://brew.sh).

Dependencies
----------------------

    brew install automake berkeley-db4 libtool boost --c++11 miniupnpc openssl pkg-config protobuf --c++11 qt5 libevent

NOTE: Building with Qt4 is still supported, however, doing so could result in a broken UI. Therefore, building with Qt5 is recommended.

Build Dash Core
------------------------
### Building Energi Core

1. Clone the bitcoin source code and cd into `dash`

        git clone https://github.com/energicryptocurrency/energi.git
        cd energi

2.  Build Energi Core:
    This will configure and build the headless Energi binaries as well as the gui (if Qt is found).
    Configure and build the headless dash binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.

        ./autogen.sh
        ./configure
        make

3.  It is recommended to build and run the unit tests:

        make check

4.  (Optional) You can also install Energi to your path:

        make deploy

4. Enter "energi-qt" as project name, enter src/qt as location
You can ignore this section if you are building `energid` for your own use.
energid/energi-cli binaries are not included in the Energi-Qt.app bundle.
If you are building `energid` or `Energi Core` for others, your build machine should be set up
Once dependencies are compiled, see [doc/release-process.md](release-process.md) for how the Energi Core
Running
-------

It's now available at `./energid`, provided that you are still in the `src`

Run `./energid` to get the filename where it should be put, or just try these

    echo -e "rpcuser=energirpc\nrpcpassword=$(xxd -l 16 -p /dev/urandom)" > "/Users/${USER}/Library/Application Support/EnergiCore/energi.conf"
    chmod 600 "/Users/${USER}/Library/Application Support/EnergiCore/energi.conf"

The first time you run dashd, it will start downloading the blockchain. This process could take several hours.

You can monitor the download process by looking at the debug.log file:

    tail -f $HOME/Library/Application\ Support/EnergiCore/debug.log

Other commands:
-------

    ./energid -daemon # to start the energi daemon.
    ./energi-cli --help  # for a list of command-line options.
    ./energi-cli help    # When the daemon is running, to get a list of RPC commands

Using Qt Creator as IDE
------------------------
You can use Qt Creator as an IDE, for dash development.
Download and install the community edition of [Qt Creator](https://www.qt.io/download/).
Uncheck everything except Qt Creator during the installation process.

1. Make sure you installed everything through Homebrew mentioned above
2. Do a proper ./configure --enable-debug
3. In Qt Creator do "New Project" -> Import Project -> Import Existing Project
4. Enter "dash-qt" as project name, enter src/qt as location
5. Leave the file selection as it is
6. Confirm the "summary page"
7. In the "Projects" tab select "Manage Kits..."
8. Select the default "Desktop" kit and select "Clang (x86 64bit in /usr/bin)" as compiler
9. Select LLDB as debugger (you might need to set the path to your installation)
10. Start debugging with Qt Creator

Notes
-----

* Tested on OS X 10.7 through 10.11 on 64-bit Intel processors only.

* Building with downloaded Qt binaries is not officially supported. See the notes in [#7714](https://github.com/bitcoin/bitcoin/issues/7714)
