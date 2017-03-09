#!/bin/sh

#If you want to share tar output (note: necessary if you want multiple archs with the same hashed tarballs)
# set SHARED_DIR - YOU MUST MAKE SURE YOU HAVE A TRAILING / ON SHARED_DIR

# If you want to pass parameters to debuild, set them in SWIFT_DEBUILD_FLAGS
# e.g. -S for just source, -b for just binaries

set -e -x

export PYTHONPATH=../../../BuildTools/SCons
VERSION=${VERSION:=`../../../BuildTools/GetBuildVersion.py swift`}
DEBIAN_VERSION=`../../../BuildTools/DebianizeVersion.py $VERSION`
DIRNAME=swift-im-$DEBIAN_VERSION
SWIFTEN_SOVERSION=`../../../BuildTools/GetBuildVersion.py swift --major`

if [ -z "$SHARED_DIR" ]; then
	echo "Using standalone output"
else
	echo "Using shared output in ${SHARED_DIR}"
fi

if [ -z "$DEBIAN_VERSION" ]; then
	echo "Unable to determine version"
	exit -1
fi

echo "Cleaning up old packages ..."
rm -f swift-im_*
rm -f libswiften*
rm -rf swift-im-*

TARBALLBARE="swift-im_$DEBIAN_VERSION.orig.tar.gz"
TARBALL="${SHARED_DIR}${TARBALLBARE}"

if [ -f $TARBALL ]; then
	echo "Found existing tarball for the right version, using ${TARBALL}"
	echo "Cleaning out any previous builds in ${DIRNAME} and re-extracting"
	rm -rf $DIRNAME
	cp $TARBALL tmp.tmp
	mv tmp.tmp $TARBALLBARE
	tar xzf $TARBALLBARE
else
	echo "No shared tarball found"
	echo "Checking out a fresh copy ..."
	rm -rf $DIRNAME
	git clone ../../../.git $DIRNAME

	# Remove development files & 3rdParty files
	rm -rf $DIRNAME/.git
	find $DIRNAME -name .gitignore | xargs rm -f
	if [ -z "$SWIFT_COPY_UUID" ]; then
		find $DIRNAME/3rdParty -type f | grep -v SConscript | xargs rm -f
	else
		find $DIRNAME/3rdParty -type f | grep -v uuid | grep -v SConscript | grep -v miniupnp | grep -v natpmp || xargs rm -f
	fi
	find $DIRNAME/3rdParty -depth -empty -type d -exec rmdir {} \;
	rm -rf $DIRNAME/3rdParty/SCons
	rm -rf $DIRNAME/Swift/Packaging/Debian
	rm -rf $DIRNAME/BuildTools/DocBook/Fonts
	rm -rf $DIRNAME/BuildTools/Git/Hooks/commit-msg

	# Initialize the build version
	echo $VERSION > $DIRNAME/VERSION.swift

	# Fork local Boost UUID copy
	# FIXME: This shouldn't be necessary, but SCons isn't picking up the generated headers for compilation
	if [ ! -z "$SWIFT_COPY_UUID" ]; then
		mkdir -p $DIRNAME/3rdParty/Boost/uuid/boost
		cp -r $DIRNAME/3rdParty/Boost/src/boost/uuid $DIRNAME/3rdParty/Boost/uuid/boost
	fi

	# Create orig tarball
	tar czf $TARBALLBARE $DIRNAME
	cp $TARBALLBARE tmp.tmp
	mv tmp.tmp $TARBALL
fi


# Detect dependencies
WEBKIT_DEPENDENCY=", libqtwebkit-dev (>= 2.0.0)"
DISTRIBUTION=`lsb_release -s -i`-`lsb_release -s -c`
if [ "$DISTRIBUTION" = "Debian-squeeze" -o "$DISTRIBUTION" = "Ubuntu-lucid" -o ! -z "$SWIFT_FORCE_LUCID" ]; then
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
cat ../../../COPYING.thirdparty | tail -n +3 >> $DIRNAME/debian/copyright
# Generate updated man-page if possible
if type "help2man" > /dev/null 2>&1; then
	if [ -f ../../QtUI/swift-im ]; then 
		help2man -m "Swift Manual" -S "Swift" -n "swift-im" -N ../../QtUI/swift-im > $DIRNAME/debian/swift-im.1
	fi
	if [ -f ../../../Swiften/Config/swiften-config ]; then
		help2man -m "Swift Manual" -S "swiften-config" -n "swiften-config" -N ../../../Swiften/Config/swiften-config > $DIRNAME/debian/swiften-config.1
	fi
else
	>2& echo "Unable to generate man pages. Please ensure that help2man is installed"
	exit 1;
fi

# Build
cd $DIRNAME
set +e
debuild -i -I ${SWIFT_DEBUILD_FLAGS} --lintian-opts --pedantic
