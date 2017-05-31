#!/usr/bin/env bash

# Returns 0 if all releases are valid. Needs to be run with sudo
# release_level is the level of the release to test.
# 2 will test development stream only
# 1 will test development and beta
# 0 will test all three development, beta and release

function finish {
  if [ "$?" -eq 0 ] && [ "$CLEAN_INSTALL" = True ]; then
    echo "No errors found, removing base systems."
    rm -rf "Debian-Tests/"
  fi
}
trap finish EXIT

if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
  echo "Usage $0 <release_tag> <release_level> [-c]"
  echo "The -c flag will recreate all base systems."
  exit 1
fi
RELEASE_LEVEL="$2"
re='^[0-9]+$'
if ! [[ "$RELEASE_LEVEL" =~ $re ]]; then
  echo "error: release level is not a number" >&2; exit 1
fi
if [ "$RELEASE_LEVEL" -gt 2 ]  || [ "$RELEASE_LEVEL" -lt 0 ]; then
  echo "error: release level should be between 0 and 2" >&2; exit 1
fi
if [ "$3" == "-c" ]; then
  echo "The script will create all base systems from scratch."
  CLEAN_INSTALL=True
else
  CLEAN_INSTALL=False
fi

declare -A arr
#Ubuntu xenial
arr[0,0]="ubuntu"
arr[0,1]="http://archive.ubuntu.com/ubuntu"
arr[0,2]="xenial"

#Ubuntu yakkety
arr[1,0]="ubuntu"
arr[1,1]="http://archive.ubuntu.com/ubuntu"
arr[1,2]="yakkety"

#Debian jessie
arr[2,0]="debian"
arr[2,1]="http://deb.debian.org/debian/"
arr[2,2]="jessie"

#Debian sid
arr[3,0]="debian"
arr[3,1]="http://deb.debian.org/debian/"
arr[3,2]="sid"

DIST="$1"
RETURN_VALUE=0

for counter in {0..3}
do
#  for ARCH in "amd64" "i386"
  for ARCH in "amd64"
  do
    echo "========================================"
    echo "Testing ${arr[${counter},0]} ${arr[${counter},2]} $ARCH"
    echo "========================================"
    DIST_FOLDER="Debian-Tests/${arr[${counter},0]}_${arr[${counter},2]}_$ARCH"
    if [ "$CLEAN_INSTALL" = True ] ; then
      rm -rf "$DIST_FOLDER"
      echo "Removing $DIST_FOLDER"
    fi
    if [ ! -d "$DIST_FOLDER" ]; then
      echo "$DIST_FOLDER was not present. Creating Base system from scratch."
      {
          mkdir -p "$DIST_FOLDER"
          debootstrap --arch=$ARCH ${arr[${counter},2]} $DIST_FOLDER ${arr[${counter},1]}
      } &>/dev/null
      echo "Setup complete."
    fi
    cp test-distr.sh "$DIST_FOLDER"

    RELEASES_COUNT=0
    for RELEASE in "development" "beta" "release"
    do
      if [[ "$RELEASES_COUNT" -le "$RELEASE_LEVEL" ]]; then
        chroot $DIST_FOLDER bash test-distr.sh https://swift.im/packages/${arr[${counter},0]}/${arr[${counter},2]} ${arr[${counter},2]} $DIST $RELEASE
        RETURN_VALUE=$((RETURN_VALUE + $?))
      else
        echo "Skipping stream: $RELEASE"
      fi
      RELEASES_COUNT=$((RELEASES_COUNT + 1))
    done
  done
done
exit $RETURN_VALUE
