#!/bin/sh

export PYTHONPATH=../../../BuildTools/SCons
VERSION=`../../../BuildTools/GetBuildVersion.py swift`
DEBIAN_VERSION=`../../../BuildTools/DebianizeVersion.py $VERSION`
DIRNAME=swift-$DEBIAN_VERSION

if [ -z "$DEBIAN_VERSION" ]; then
	echo "Unable to determine version"
	exit -1
fi

echo "Cleaning up old packages ..."
rm swift_*

echo "Checking out a fresh copy ..."
rm -rf $DIRNAME
git clone ../../../.git $DIRNAME
ln -s Swift/Packaging/Debian/debian $DIRNAME/debian
../../../BuildTools/UpdateDebianChangelog.py $DIRNAME/debian/changelog $DEBIAN_VERSION

echo "Building ..."
cd $DIRNAME
echo $PWD
dpkg-buildpackage -rfakeroot -i -I
