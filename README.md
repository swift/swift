# Swift - An elegant, secure, adaptable and intuitive XMPP Client
## Quick Start
Simply run `./scons Swift` on Linux/Mac or `scons.bat Swift` on Windows to build Swift.

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
* Swift
    * Windows 7 and later
    * Mac OS X 10.10 and later
    * Linux distributions. We usually test on default Ubuntu and Debian installations.
* Swiften (same as above)
    * Android
    * iOS

## External Dependencies
The Swift repository includes some third party dependencies in the 3rdParty directory
to easy development. Third party dependencies not included are listed below:

* OpenSSL is required when building Swiften on Linux or Android
* Qt 5 is required by Swift; **Qt up to version Qt 5.5 is supported**

## General Build Instructions
The Swift projects use the SCons build system for build configuration and Build
execution. SCons is one of the included third party dependencies.

To build all Swift components simply run SCons. On Mac OS X and Linux you run the `./scons` command. On Windows you run the `scons.bat` command in a Visual Studio developer shell.

On multi-processor systems, i.e. most desktop and server machines, you can use the `-j NUMBER_OF_PARALLEL_TASKS` flag to speed up build time.

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

A binary release of Qt can be obtained from https://www.qt.io/download-open-source/. Make sure it matches the Visual Studio version you have installed.

#### Building Swift

From the 'Visual C++' 'Programs' group, launch the Visual C++ command prompt, go to your checked out Swift directory and run `scons.bat qt=C:\path\to\qt Swift` to build Swift.

To start Swift, simply change to `Swift/QtUI` and run `Swift.exe`.

### Unix / Linux
#### Prerequisites
On Linux Swift mainly depends on Qt and OpenSSL.
To install all required dependencies take a look at `./BuildTools/InstallSwiftDependencies.sh`. It is a script that supports various Linux distributions and will install the required system packages for building and running Swift.

On Unix/Linux SCons will use `pkg-config` to determine the compiler and linker flags for Qt 5.

Note: The Qt that comes with your Linux distribution might not be up to date and may have issues with some desktop environments.

### Building Swift
To build Swift, simply open a terminal, go to the project root and run `./scons Swift`. Afterwards you can start Swift by running `./Swift/QtUI/swift-im`.
