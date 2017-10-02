#!/bin/sh

APP=$1
TEMPLATE=$2
TARGET=$3
QTDIR=$4
CODESIGN_IDENTITY=$5

if [[ ! -f "$TEMPLATE" || ! -d "$APP" || ! -d "$QTDIR" || -z "$TARGET" ]]; then
	echo "Error"
	exit -1
fi

set -e -x

WC_DIR=`dirname $TEMPLATE`/tmp
WC_DMG=`dirname $TEMPLATE`/tmp.dmg

gunzip -c $TEMPLATE > $WC_DMG
rm -rf $WC_DIR
mkdir -p $WC_DIR
hdiutil attach "$WC_DMG" -noautoopen -quiet -mountpoint "$WC_DIR"
ditto -rsrc "$APP" "$WC_DIR"/`basename $APP`
$QTDIR/bin/macdeployqt "$WC_DIR"/`basename $APP` -no-strip 

# The bearer plugins of Qt5 cannot be disabled by an application. 
# They will constantly ask the OS to scan for available wireless networks, unrelated to actual API calls.
# This results in unnessecary resource load, so we remove the plugins from our packages.
rm "$WC_DIR"/`basename $APP`/Contents/PlugIns/bearer/*.dylib

# Remove debugging symbols from the application bundle
rm -rf "$WC_DIR"/`basename $APP`/Contents/MacOS/*.dSYM

# Codesign the app bundle if requested by caller
if [ ! -z "$CODESIGN_IDENTITY" ]; then
	codesign --verbose --force --deep --sign "$CODESIGN_IDENTITY" "$WC_DIR"/`basename $APP`
fi

hdiutil detach "$WC_DIR" -quiet -force
rm -f $TARGET
hdiutil convert "$WC_DMG" -quiet -format UDZO -imagekey zlib-level=9 -o "$TARGET"
rm -rf $WC_DMG $WC_DIR
