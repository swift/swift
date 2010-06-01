#!/bin/sh

export PYTHONPATH=../../../BuildTools/SCons
VERSION=`../../../BuildTools/GetBuildVersion.py swift`
DIRNAME=swift-$VERSION

rm -rf $DIRNAME
git clone ../../../.git $DIRNAME
ln -s Swift/Packaging/Debian/debian $DIRNAME/debian

cd $DIRNAME
echo $PWD
dpkg-buildpackage -rfakeroot -i -I
