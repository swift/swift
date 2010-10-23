#!/bin/sh

APP=$1
TEMPLATE=$2
TARGET=$3
QTDIR=$4

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
$QTDIR/bin/macdeployqt "$WC_DIR"/`basename $APP`
hdiutil detach "$WC_DIR" -quiet -force
rm -f $TARGET
hdiutil convert "$WC_DMG" -quiet -format UDZO -imagekey zlib-level=9 -o "$TARGET"
rm -rf $WC_DMG $WC_DIR
