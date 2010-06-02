#!/bin/sh

export PYTHONPATH=../../../BuildTools/SCons
VERSION=`../../../BuildTools/GetBuildVersion.py swift`
DIRNAME=swift-$VERSION

if [ -z "$VERSION" ]; then
	echo "Unable to determine version"
	exit -1
fi

echo "Cleaning up old packages ..."
rm swift_*

echo "Checking out a fresh copy ..."
rm -rf $DIRNAME
git clone ../../../.git $DIRNAME
ln -s Swift/Packaging/Debian/debian $DIRNAME/debian

echo "Building ..."
cd $DIRNAME
echo $PWD
dpkg-buildpackage -rfakeroot -i -I
