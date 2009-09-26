VERSION=1.0
PACKAGE=swift-$VERSION

rm -rf $PACKAGE
git archive --format=tar --prefix=$PACKAGE/ HEAD | tar x
cp -r Swift/Packaging/Debian $PACKAGE/debian
pushd $PACKAGE
SWIFT_VERSION=$VERSION dpkg-buildpackage -rfakeroot
popd
rm -rf $PACKAGE
