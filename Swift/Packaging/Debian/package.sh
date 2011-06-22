#!/bin/sh

set -e -x

export PYTHONPATH=../../../BuildTools/SCons
VERSION=${VERSION:=`../../../BuildTools/GetBuildVersion.py swift`}
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

# Remove development files & 3rdParty files
rm -rf $DIRNAME/.git
find $DIRNAME -name .gitignore | xargs rm -f
find $DIRNAME/3rdParty -type f | grep -v uuid | grep -v SConscript | xargs rm -f
find $DIRNAME/3rdParty -depth -empty -type d -exec rmdir {} \;
rm -rf $DIRNAME/3rdParty/SCons
rm -rf $DIRNAME/Swift/Packaging/Debian

# Initialize the build version
echo $VERSION > $DIRNAME/VERSION.swift

# Fork local Boost UUID copy
# FIXME: This shouldn't be necessary, but SCons isn't picking up the generated headers for compilation
mkdir -p $DIRNAME/3rdParty/Boost/uuid/boost
cp -r $DIRNAME/3rdParty/Boost/src/boost/uuid $DIRNAME/3rdParty/Boost/uuid/boost

# Create orig tarball
tar czf swift-im_$DEBIAN_VERSION.orig.tar.gz $DIRNAME

# Detect dependencies
WEBKIT_DEPENDENCY=", libqtwebkit-dev (>= 2.0.0)"
DISTRIBUTION=`lsb_release -s -i`-`lsb_release -s -c`
if [ "$DISTRIBUTION" = "Debian-squeeze" -o "$DISTRIBUTION" = "Ubuntu-lucid" ]; then
	WEBKIT_DEPENDENCY=""
fi

# Initialize debian files
cp -r debian $DIRNAME/debian
if [ -z "$CHANGELOG" ]; then
	../../../BuildTools/UpdateDebianChangelog.py $DIRNAME/debian/changelog $DEBIAN_VERSION
else
	cp $CHANGELOG $DIRNAME/debian/changelog
fi
cat $DIRNAME/debian/control.in | sed -e "s/%SWIFTEN_SOVERSION%/$SWIFTEN_SOVERSION/g" | sed -e "s/%WEBKIT_DEPENDENCY%/$WEBKIT_DEPENDENCY/g" > $DIRNAME/debian/control
rm $DIRNAME/debian/control.in
mv $DIRNAME/debian/libswiften.install $DIRNAME/debian/libswiften$SWIFTEN_SOVERSION.install
cat ../../../COPYING | awk '/--- END OF OpenSSL/,EOF' | tail -n +3 >> $DIRNAME/debian/copyright

# Build
cd $DIRNAME
set +e
debuild -i -I --lintian-opts --pedantic
