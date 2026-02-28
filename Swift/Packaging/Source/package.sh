#!/bin/sh

## For a Swiften-only tarball, try
## EXCLUDE_FOLDERS="Swift Sluift Limber Slimber" ./package.sh

## This script will require GPG and a suitable signing key by default.
## To disable tarball signature generation, pass the --nosign parameter to package.sh.

export PYTHONPATH=../../../BuildTools/SCons
VERSION=`../../../BuildTools/GetBuildVersion.py swift`
DIRNAME=swift-$VERSION
FIRST_ARGUMENT=$1

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

if [ "$FIRST_ARGUMENT" != "--nosign" ]; then
	echo "Creating armored GPG signature ..."
	which gpg
	if [ $? -eq 0 ]; then
		gpg --list-keys packages@swift.im
		if [ $? -eq 0 ]; then
			gpg --armor --default-key packages@swift.im --detach-sign $DIRNAME.tar.gz
		else
			echo "Key for packages@swift.im missing. Not signing tar archive."
			exit 1
		fi
	else
		echo "GPG not found. Not signing tar archive."
		exit 1
	fi
fi