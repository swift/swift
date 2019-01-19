#!/bin/bash

set -e

cd $(dirname $(readlink -f $0 || realpath $0))
export PYTHONPATH=../../../BuildTools/SCons
VERSION=${VERSION:=`../../../BuildTools/GetBuildVersion.py swift`}
DEBIAN_VERSION=`../../../BuildTools/DebianizeVersion.py $VERSION`
SWIFTEN_SOVERSION=`../../../BuildTools/GetBuildVersion.py swift --major`
SWIFT_SOURCE_DIR=swift-im-$DEBIAN_VERSION
TARBALLBARE="swift-im_$DEBIAN_VERSION.orig.tar.gz"
TARBALL="${SHARED_DIR}${TARBALLBARE}"
# DEVATTN there is an issue with some versions of Qt(5.11.1) and docker see https://bugreports.qt.io/browse/QTBUG-64490. Workaround is to add privileged flag
if [ -z ${DOCKERRUNEXTRAFLAGS+x} ];
then
	DOCKERRUNEXTRAFLAGS="--privileged"
fi

rm -rf $SWIFT_SOURCE_DIR
git clone ../../../.git $SWIFT_SOURCE_DIR

rm -rf $SWIFT_SOURCE_DIR/.git
find $SWIFT_SOURCE_DIR -name .gitignore | xargs rm -f
find $SWIFT_SOURCE_DIR/3rdParty -type f | grep -v uuid | grep -v SConscript | grep -v miniupnp | grep -v natpmp || xargs rm -f
find $SWIFT_SOURCE_DIR/3rdParty -depth -empty -type d -exec rmdir {} \;
rm -rf $SWIFT_SOURCE_DIR/3rdParty/SCons
rm -rf $SWIFT_SOURCE_DIR/Swift/Packaging/Debian
rm -rf $SWIFT_SOURCE_DIR/BuildTools/DocBook/Fonts
rm -rf $SWIFT_SOURCE_DIR/BuildTools/Git/Hooks/commit-msg
echo $VERSION > $SWIFT_SOURCE_DIR/VERSION.swift

tar czf $TARBALLBARE $SWIFT_SOURCE_DIR
cp $TARBALLBARE $SWIFT_SOURCE_DIR

cp -r debian $SWIFT_SOURCE_DIR/debian
../../../BuildTools/UpdateDebianChangelog.py $SWIFT_SOURCE_DIR/debian/changelog $DEBIAN_VERSION

cat $SWIFT_SOURCE_DIR/debian/control.in | sed -e "s/%SWIFTEN_SOVERSION%/$SWIFTEN_SOVERSION/g" | sed -e "s/%WEBKIT_DEPENDENCY%/$WEBKIT_DEPENDENCY/g" > $SWIFT_SOURCE_DIR/debian/control
rm $SWIFT_SOURCE_DIR/debian/control.in
mv $SWIFT_SOURCE_DIR/debian/libswiften.install $SWIFT_SOURCE_DIR/debian/libswiften$SWIFTEN_SOVERSION.install
cat ../../../COPYING.thirdparty | tail -n +3 >> $SWIFT_SOURCE_DIR/debian/copyright

rm -rf swift-src
mv $SWIFT_SOURCE_DIR swift-src
docker build -t swift/swiftsourcebuilder -f Dockerfile.source .
mkdir -p out/source
docker run --rm -v`pwd`/gpg-keys:/gpg-keys  -v`pwd`/out/source:/out swift/swiftsourcebuilder

TMP_DOCKERFILE=Dockerfile.tmp.package

if [ -z ${SWIFT_PACKAGE_PLATFORMS+x} ];
then
	platformsarray=( ubuntu:bionic ubuntu:cosmic debian:stretch debian:sid )
else
	platformsarray=( $SWIFT_PACKAGE_PLATFORMS )
fi

for distro in "${platformsarray[@]}";
do
    if [ -f $TMP_DOCKERFILE ];
    then
        rm $TMP_DOCKERFILE
    fi
    cat Dockerfile.package.in | sed -e "s/__DISTRO__/$distro/" -e "s/__DEBIAN_VERSION__/$DEBIAN_VERSION/" > $TMP_DOCKERFILE
    docker build -t swift/packager -f $TMP_DOCKERFILE out/source
    OUT_DIR="out/${distro//\://}"
    echo $OUT_DIR
    mkdir -p $OUT_DIR
    docker run --rm $DOCKERRUNEXTRAFLAGS -v`pwd`/gpg-keys:/gpg-keys -v`pwd`/$OUT_DIR:/out swift/packager
done
