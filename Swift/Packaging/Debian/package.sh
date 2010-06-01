#!/bin/sh

VERSION=`../../../GetBuildVersion.py swift`
DIRNAME=swift-$VERSION

rm -rf $DIRNAME
git clone ../../../.git $DIRNAME
ln -s Swift/Packaging/Debian/debian $DIRNAME/debian

cd $DIRNAME
echo $PWD
dpkg-buildpackage -rfakeroot -i -I
