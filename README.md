# Swift - An elegant, secure, adaptable and intuitive XMPP Client

__NOTE: Please download any releases from https://swift.im/releases.html, or as a Git checkout. The tarballs provided by GitHub will not build correctly.__

## Quick Start
Simply run `./scons Swift` on Linux, or `./scons Swift qt=/Users/username/Qt_installation_path/Qt/5.4.2/5.4/clang_64` on Mac OS X or `scons.bat Swift qt=C:\path\to\qt` on Windows to build Swift.

This document only covers the general structure, project dependencies and building the Swift GUI application.
More detailed information on building components, development, testing and packaging can be found in DEVELOPMENT.md.

## Components
* Swiften: A cross-platform C++ XMPP library.
* Swift: A Qt-based cross-platform desktop GUI client based on Swiften.
* Sluift: A XMPP scripting environment based on Lua and Swiften.
* Limber: A XMPP proxy server proxying zero-configuration (XEP-0174) to a
connected XMPP client.
* Slimber: Qt and Cocoa frontends for Limber.
* SwiftTools: A collection of utilities for Swift.

## Supported Platforms
Swift is known to work on Windows, Mac OS X, Linux and other Unix-like systems. However, we do not test the whole range of operating systems, especailly the huge amount of Linux and Unix-like systems.
Swiften further has support for Android and iOS.

Platforms we officially test our releases on are listed below. We only test on default configurations and default desktops environments on Linux distributions.

* Swift
    * Windows 7 to Windows 10
    * Mac OS X 10.10 and Mac OS X 10.11
    * Ubuntu 17.10 ( Artful Aardvark ) and Ubuntu 16.04 ( Xenial Xerus )
    * Debian 8 ( jessie )
    * Debian 9 ( stretch )

## External Dependencies
The Swift repository includes some third party dependencies in the 3rdParty directory
to easy development. Third party dependencies not included are listed below:

* OpenSSL is required when building Swiften on Linux or Android
* Python (2.5 <= version < 3)
* Qt 5 and QtWebKit is required by Swift. Depending on platform, architecture, and compiler/IDE, you have the following options to fulfill this dependency:
  * download and install the [official binary Qt 5.5 release](http://download.qt.io/archive/qt/5.5/5.5.1/) if it is available for your platform/architecture/compiler
  * download and install the latest [official binary Qt 5.7 release](http://download.qt.io/archive/qt/5.7/) **and** download the *matching* [Qt WebKit Technology Preview release for Qt 5.7](https://github.com/annulen/webkit/releases/) and copy/merge the folders with the matching folders of the previously installed Qt 5.7 installation
  * Some features of Swift, such as unicode Emojis support, require QT 5.6 or newer.

## General Build Instructions
The Swift projects use the SCons build system for build configuration and Build
execution. SCons is one of the included third party dependencies.

To build all Swift components simply run SCons. On Mac OS X and Linux you run the `./scons` command. On Windows you run the `scons.bat` command in a Visual Studio developer shell.

On multi-processor systems, i.e. most desktop and server machines, you can use the `-j NUMBER_OF_PARALLEL_TASKS` flag, or pass the `max_jobs=1` to the `scons` command to speed up build time. The `max_jobs=1` argument will set the number of parallel tasks to the number of CPU cores available. Note that each parallel task increases the memory requirements for the overall build process.

## Platform Specific Instructions
### Mac OS X
#### Prerequisites
- XCode (C++ 11 compiler) or Clang
- Qt 5: The binary release of Qt provided on https://www.qt.io/download-open-source/ works just fine.

#### Building Swift

After installing Qt, simply run `./scons qt=/Users/username/Qt_installation_path/Qt/5.4.2/5.4/clang_64 Swift` to build Swift. Afterwards you can find a runnable app bundle at `Swift/QtUI/Swift.app`.

### Windows
#### Prerequisites
- Microsoft Visual Studio 2013 Express or later (C++11)
- Windows SDK
- Python (2.5 <= version < 3)
- Qt Open Source Edition
  - QT 5.6 or newer to support emojis.

A binary release of Qt can be obtained from https://www.qt.io/download-open-source/. Make sure it matches the Visual Studio version you have installed.

#### Building Swift

Open a Developer Command Prompt. Depending on Windows version and Visual Studio version, this can be found in the **Visual C++ Programs group**, launch the **Visual C++ command prompt**, or search for **Developer Command Prompt for VS2015** in the Windows start menu in more recent versions of Windows.

In the open command prompt add the **Qt installation bin folder** to the `PATH` environment variable, by running `set PATH=C:\path\to\qt\msvc2015\bin;%PATH%`. The Qt path is just an example; adjust it to your local installation.

Afterwards, in the command prompt, go to your checked out Swift directory and run `scons.bat qt=C:\path\to\qt Swift` to build Swift. If you want to build a 64-bit version of Swift, **this requires to have 64-bit versions of all dependencies**, attach `win_target_arch=x64_64` to the previous `scons` command line. 

To start Swift, simply change to `Swift/QtUI` and run `Swift.exe`.

### Unix / Linux
#### Prerequisites
On Linux Swift mainly depends on Qt and OpenSSL.
To install all required dependencies take a look at `./BuildTools/InstallSwiftDependencies.sh`. It is a script that supports various Linux distributions and will install the required system packages for building and running Swift.

On Unix/Linux SCons will use `pkg-config` to determine the compiler and linker flags for Qt 5.

Note: The Qt that comes with your Linux distribution might not be up to date and may have issues with some desktop environments.

#### Building Swift
To build Swift, simply open a terminal, go to the project root and run `./scons Swift`. Afterwards you can start Swift by running `./Swift/QtUI/swift-im`.

#### Installing Swift
To install Swift on the system (e.g. in the `/usr/local` directory), run `./scons SWIFT_INSTALLDIR=/usr/local /usr/local` in the Swift project folder.
