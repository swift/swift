#!/bin/sh

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

echo "Creating tarball ..."
tar czf $DIRNAME.tar.gz $DIRNAME
