#!/bin/sh
# This script removes parts of the breakpad repository that are not used by swift

rm -rf README.ANDROID
rm -rf .travis.yml
rm -rf aclocal.m4
rm -rf appveyor.yml
rm -rf configure
rm -rf configure.ac
rm -rf Makefile.am
rm -rf Makefile.in
rm -rf breakpad.pc
rm -rf codereview.settings
rm -rf breakpad-client.pc.in
rm -rf DEPS INSTALL
rm -rf .git/
rm -rf m4/
rm -rf docs/
rm -rf autotools/
rm -rf android/
rm -rf scripts/
rm -rf src/build/
rm -rf src/third_party
rm -rf src/tools
rm -rf src/client/ios/
rm -rf src/client/apple/
rm -rf src/client/linux/
rm -rf src/client/solaris/
rm -rf src/client/mac/Framework
rm -rf src/client/mac/Breakpad.xcodeproj
rm -rf src/client/mac/sender
rm -rf src/client/mac/testapp
rm -rf src/client/mac/tests
rm -rf src/client/mac/UnitTests-Info.plist
rm -rf src/client/mac/handler/minidump_test.xcodeproj/project.pbxproj
rm -rf src/client/mac/handler/minidump_tests32-Info.plist
rm -rf src/client/mac/handler/minidump_tests64-Info.plist
rm -rf src/client/mac/handler/obj-cTestCases-Info.plist
rm -rf src/client/mac/handler/testcases/
rm -rf src/client/windows/unittests
rm -rf src/client/windows/sender
rm -rf src/client/windows/tests
rm -rf src/client/windows/breakpad_client.gyp
rm -rf src/client/windows/handler/exception_handler.gyp
rm -rf src/client/windows/crash_generation/crash_generation.gyp
rm -rf src/client/mac/handler/testcases/testdata/
rm -rf src/common/linux/tests/
find src/common/linux -type f ! -name 'linux_libc_support.h' -delete
rm -rf src/common/windows/common_windows.gyp
rm -rf src/common/android/
rm -rf src/common/solaris/
rm -rf src/common/tests/
rm -rf src/common/testdata/
rm -rf src/common/mac/testing/
rm -rf src/common/common.gyp
rm -rf src/processor
rm -rf src/google_breakpad/processor

