#!/bin/sh

set -e -x

export PYTHONPATH=../../../BuildTools/SCons
VERSION=`../../../BuildTools/GetBuildVersion.py swift`
DEBIAN_VERSION=`../../../BuildTools/DebianizeVersion.py $VERSION`
DIRNAME=swift-im-$DEBIAN_VERSION
SWIFTEN_SOVERSION=`../../../BuildTools/GetBuildVersion.py swift --major`

if [ -z "$DEBIAN_VERSION" ]; then
	echo "Unable to determine version"
	exit -1
fi

echo "Cleaning up old packages ..."
rm -f swift-im_*
rm -f libswiften*
rm -rf swift-im-*

echo "Checking out a fresh copy ..."
rm -rf $DIRNAME
git clone ../../../.git $DIRNAME
find $DIRNAME/3rdParty/Boost/src -type f | grep -v uuid | xargs rm -f
find $DIRNAME/3rdParty/SCons -delete
find $DIRNAME/3rdParty/DocBook -delete
# FIXME: This shouldn't be necessary, but SCons isn't picking up the generated headers for compilation
mkdir -p $DIRNAME/3rdParty/Boost/uuid/boost
cp -r $DIRNAME/3rdParty/Boost/src/boost/uuid $DIRNAME/3rdParty/Boost/uuid/boost

ln -s Swift/Packaging/Debian/debian $DIRNAME/debian
../../../BuildTools/UpdateDebianChangelog.py $DIRNAME/debian/changelog $DEBIAN_VERSION
cat $DIRNAME/debian/control.in | sed -e "s/%SWIFTEN_SOVERSION%/$SWIFTEN_SOVERSION/g" > $DIRNAME/debian/control
mv $DIRNAME/debian/libswiften.install $DIRNAME/debian/libswiften$SWIFTEN_SOVERSION.install
mv $DIRNAME/debian/libswiften-dev.install $DIRNAME/debian/libswiften$SWIFTEN_SOVERSION-dev.install

echo "Building ..."
cd $DIRNAME
set +e
dpkg-buildpackage -S -rfakeroot -i -I
dpkg-buildpackage -b -rfakeroot -i -I
