# Swift Development Overview
## Build System
It is also possible to set frequently used configuration options in a `config.py`
file in the project folder.
Settings of particular interest are:
- `qt` to set the path to a Qt installation
- `cc`, `cxx`, `link` to set the C compiler, C++ compiler and linker to use
- `cxxflags` to set additional flags to pass to the C++ compiler
- `linkflags` to set additional flags to pass to the linker

If you change anything in the configuration, or install previously missing dependencies, you might need to pass `force-configure=1` to SCons to force it to
look for the dependencies again and not use cached information.

It is also possible to only build Swiften or Sluift by passing explicit build targets to scons. For example `./scons Swiften` to only build Swiften.

To install the build output on the system, simply run `./scons SWIFTEN_INSTALLDIR=/usr/local /usr/local

Swift is written in cross-platform C++11. It is continuously tested with GCC, Clang and Visual Studio 2013.

## Testing
Swift has unit and integration tests. To run the unit tests, pass `test=unit`, and to run the integration tests, pass `test=system`, or pass `test=all` to run both.

The integration tests require external setup and parameters to run.
- `SWIFT_CLIENTTEST_JID=user@domain`
- `SWIFT_CLIENTTEST_PASS=password`
- `SWIFT_FILETRANSFERTEST_JID=user@domain`
- `SWIFT_FILETRANSFERTEST_PASS=password`
- `SWIFT_FILETRANSFERTEST2_JID=anotheruser@domain`
- `SWIFT_FILETRANSFERTEST2_PASS=password`
- `SWIFT_NETWORK_TEST_IPV4=0.0.0.0`

Note: The unit and integration tests are build targets. When passing specific build targets to SCons the tests will not be build and run (even with `test=all`) unless the build targets for the tests are added as arguments to the `scons` call.

## Packaging
### Mac OS X
The Mac OS X package consists of a disk image (.dmg) file containing an independently deployable **Swift.app** application bundle.

For creating the Mac OS X package, i.e. the disk image, simply call `./scons.sh dist=1`. To additionally code sign the **Swift.app** application
bundle, append `codesign_identity="Developer ID Application: Something here"` with your developer certificate identity to the `scons` command.

**WARNING: Code signing has not been tested with third-party signed certificates yet!**

### Windows
The Windows package consist of a Widnows Installer (.msi) file. This is generated using the Windows Installer XML Toolset.

For packaging use:
- Microsoft VS 2013 Express
- WiX
- Download the C++ redistributable package from Microsoft and put it at `C:\Program Files (x86)\Common Files\Merge Modules\`
- `config.py` should contain:
    qt = "c:\\qt\\5.4.2"
    vcredist = "C:\\Program Files (x86)\\Common Files\\Merge Modules\\vcredist_x86.exe"
    debug = 1
    optimize = 1
    wix_bindir = "c:\\program files\\Windows Installer XML v3.5\\bin"
- Run the following commands. Two scons runs are required as 'scons dist=1' requires the Swift.exe to be in place.
```
scons
scons dist=1
```

To sign the resulting MSI file, append the `signtool_key_pfx` and `signtool_timestamp_url` parameters to the `scons dist=1` programm call, e.g.
`scons dist=1 signtool_key_pfx=C:\Users\Swift\SwiftSPC.pfx signtool_timestamp_url=http://timestamp.verisign.com/scripts/timstamp.dll`.

Notes:
- The settings `debug = 1` and `optimize = 1` are **strictly required** if you use a precompiled Qt release from the Qt Project; otherwise you will get linker errors
- On 64-bit Windows it's "Program Files (x86)" instead of "Program Files" in the
  paths
- Currently only 32-bit builds of the Swift client are supported

## Automatic Software Updates
Automatic software updates allow distribution of updates directly to the end users.
This is useful for general feature updates, bug fixes and especially for security
updates.

### Automatic Software Updates for Mac OS X using Sparkle
Swift supports integration with the software update framework [Sparkle](https://sparkle-project.org/) on OS X. For security reasons,
Sparkle requires the application to be either code-signed or a bundled public DSA
key. In case you do not code-sign, you can provide the path to the public DSA key
to be bundled with the application bundle via the `sparkle_public_dsa_key` SCons
argument.

To build with Sparkle support, simply download Sparkle-1.14.0 and extract it to
`3rdParty/Sparkle/Sparkle-1.14.0`. SCons will pick it up during configuration
and build Swift with Sparkle support.

The appcast URLs are specified in `Swift/QtUI/SwiftUpdateFeeds.h`.

## Building Swiften for Android
This section describes how to build Swiften for Android. It can then be used from any Android native code. This guide has been tested on OS X and Linux.

### Prerequisites
- Android SDK
- Android NDK
- Python
- Other requirements that are obtained during this guide
  - LDNS
  - Unbound
  - OpenSSL

### Note on Supported Features
File-transfer support has not been tested on Android. Link-local features are not supported under Android.

### Supported Android NDK Level
The minimal supported Android NDK API level is 14 (see --platform parameter of make-standalone-toolchain.sh).
This is the NDK API level associated with Android 4.0. Higher Android NDK API levels however should work.

### Preparation of Build Environment
1. Fetch 3rd-party dependencies
    1. LDNS
    ```
    cd 3rdParty/Ldns
    bash prepare_ldns.sh
    ```
    2. Unbound
    ```
    cd 3rdParty/Unbound
    bash prepare_unbound.sh
    ```

    3. OpenSSL
    ```
    cd 3rdParty/OpenSSL
    curl -O http://www.openssl.org/source/openssl-1.0.1j.tar.gz
    tar -xf openssl-1.0.1j.tar.gz
    mv openssl-1.0.1j openssl
    ```

2. Patch 3rd-party dependencies
    ```
    patch -p0 < 3rdParty/Ldns/01_andoird_compilation_fixes.diff
    patch -p1 < 3rdParty/LibMiniUPnPc/01_android_compilation_fixes.diff
    patch -p1 < 3rdParty/Unbound/01_android_fixes.diff
    ```

3. Create Android NDK Toolchain
    ```
    cd $ANDROID_NDK_ROOT (this is the folder where you extracted Android NDK)
    bash ./build/tools/make-standalone-toolchain.sh --platform=android-14 --install-dir=$HOME/android-14-tc
    ```

### Configuring Swift Build
1. Set config.py to (probably other paths, replace $NDK_TOOLCHAIN_INSTALL_DIR with the expanded path of the install dir used in the previous step):
```
android_toolchain=$NDK_TOOLCHAIN_INSTALL_DIR
android_sdk_bin="/usr/local/bin"
target = "android"
unbound = 1
```

### Building
Simply run `./scons test=none Swiften QA max_jobs=1`.

### Testing
Running Swift's test suite on Android requires a one time preparation of a testing environment.
This creates an Android VM where the test will be deployed to and run on.

1. Preparation of test environment (one time)
    1. Starting AVD (Android Device Manager) by running `android avd`
    2. Create a test VM by clicking the "Create..." button
    3. Fill out dialog
        - AVD Name: swift_test
        - Device: Nexus 4
        - Target: Anything with API level equal or greater than your toolchain you build
        - CPU/ABI: ARM (unless you build a different toolchain)
    4. Click "Ok" button
    5. Close AVD

2. Running integration and unit tests
    To run the unit tests you need to have an Android VM running while the test suite is running.
    This requires to leave the command under step 2.1 running while you execute step 2.2.

    Running the test suite on Android can take quite some time ( >30 minutes ) compared to a native run.
    This is due to a) the slow transfer speed from host machine to VM (~ 2 MB/s) and and b) the test environment being emulated for an different CPU architecture.

    1. Start Android VM with resized /system partition:
    `emulator -partition-size 2048 @swift_test`

    2. Run integration and unit tests (wait until the android boot screen is gone)
    `env SWIFT_CLIENTTEST_JID="some jabber id" SWIFT_CLIENTTEST_PASS="password for the jabber id" ./scons test=all Swiften Swift/Controllers QA`

## Building Swiften for iOS
### Prerequisites
- XCode
- Python
- Other requirements that are obtained during this guide
    - OpenSSL

### Supported Components
- Swiften

  Note: File-transfer support has not been tested on iOS. Link-local features are not supported under iOS.
- Swift/Controllers

### Preparation of Build Environment
1. Fetch 3rd-party dependencies
    1. OpenSSL
    ```
    cd 3rdParty/OpenSSL
    curl -O http://www.openssl.org/source/openssl-1.0.1j.tar.gz
    tar -xf openssl-1.0.1j.tar.gz
    mv openssl-1.0.1j openssl
    ```

### Building
1. Switch to iOS directory
    ```
    cd BuildTools/iOS
    ```

2. Look up installed SDKs
    ```
    xcodebuild -showsdks
    ```
    Note: Here the iOS SDKs and iOS Simulator SDKs are of interest.

3. Build Swiften (adjust the sdk parameter to one of the installed SDKs)
    ```
    xcodebuild build -arch i386 -sdk iphonesimulator8.1
    ```

    Note: When not specifying the architecture (-arch flag) XCode will try an universal build which is not supported.

    Only specify one architecture at a time and if you require a universal lib you can create one using the `lipo` tool.

4. The final libSwiften.a is located at `$SWIFT/build/current/Swiften/libSwiften.a`.

### Testing
Integration and unit tests are not supported on iOS.
