#!/bin/sh

## For a Swiften-only tarball, try
## EXCLUDE_FOLDERS="Swift Sluift Swiftob Limber Slimber" ./package.sh

export PYTHONPATH=../../../BuildTools/SCons
VERSION=`../../../BuildTools/GetBuildVersion.py swift`
DIRNAME=swift-$VERSION

if [ -z "$VERSION" ]; then
	echo "Unable to determine version"
	exit -1
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
tar czf $DIRNAME.tar.gz $DIRNAME
