#!/bin/sh

## For a Swiften-only tarball, try
## EXCLUDE_FOLDERS="Swift Sluift Limber Slimber" ./package.sh

export PYTHONPATH=../../../BuildTools/SCons
VERSION=`../../../BuildTools/GetBuildVersion.py swift`
DIRNAME=swift-$VERSION

if [ -z "$VERSION" ]; then
	echo "Unable to determine version"
	exit -1
fi

if [ "`uname -s`" = "Darwin" ]; then
	TAR=gtar
	if ! $TAR --version >/dev/null 2>&1; then
		echo "Unable to find GNU tar (coulnd't find $TAR)"
		exit 1
	fi
else
	TAR=tar
fi

echo "Cleaning up old sources ..."
rm -rf swift-*

echo "Checking out a fresh copy ..."
rm -rf $DIRNAME
git clone ../../../.git $DIRNAME
echo "$VERSION" > $DIRNAME/VERSION.swift
rm -rf $DIRNAME/.git

echo "Excluding folders based on environment variable EXCLUDE_FOLDERS='$EXCLUDE_FOLDERS'"
for FOLDER in $EXCLUDE_FOLDERS;
do
  echo "Excluding $FOLDER"
  rm -rf $DIRNAME/$FOLDER
done

echo "Creating tarball ..."
$TAR -czf $DIRNAME.tar.gz --owner=swift --group=swift $DIRNAME
