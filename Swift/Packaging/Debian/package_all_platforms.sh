#!/bin/bash

# Quit immediately if any part fails
set -e

## To get to this stage you need to already have the pbuilder-dist stuff installed, and have already run:
# for distro in xenial yakkety jessie sid; do
#  for arch in amd64; do
#   pbuilder-dist $distro $arch create
#  done
# done

export SWIFT_DEBUILD_FLAGS="-S"
export SWIFT_FORCE_LUCID="yep"

#./package.sh

#for distro in lucid squeeze; do
#	for arch in i386 amd64; do
#		pbuilder-dist $distro $arch build *.dsc
#	done
#done

unset SWIFT_FORCE_LUCID
./package.sh

if [ -z ${SWIFT_PACKAGE_PLATFORMS+x} ];
then
	platformsarray=( xenial artful jessie stretch sid )
else
	platformsarray=( $SWIFT_PACKAGE_PLATFORMS )
fi

for distro in "${platformsarray[@]}"; do
	for arch in amd64; do
		pbuilder-dist $distro $arch build *.dsc
	done
done
