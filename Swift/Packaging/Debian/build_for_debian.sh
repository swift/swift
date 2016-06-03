#!/bin/bash

echo "** CHECK YOU'VE UPDATED THE CHANGELOG **"
export CHANGELOG=changelog.debian-unstable
export SWIFT_DEBUILD_FLAGS="-S"

./package.sh

for arch in amd64; do
	pbuilder-dist sid $arch build *.dsc
done
