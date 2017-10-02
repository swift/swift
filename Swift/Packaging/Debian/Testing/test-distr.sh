#!/usr/bin/env bash

# Return values
# 0 if the package, source and installation were successful
# 1 if the source packages are missing
# 2 if the binary is not present in the repository
# 3 if the binary and source packages are not present in the repository
# 4 if the binary failed to install
# 5 if the binary failed to install and the source package is not available.

function finish {
  rm /etc/apt/sources.list.d/swift-distr.list
  if [[ "$1" != "*ubuntu*" ]]; then
    rm /etc/apt/sources.list.d/ubuntu-universe.list
  fi
  {
    rm -rf swift-im* packages.key*
    apt-get remove -y swift-im
    apt-get autoremove -y
  } &>/dev/null
}
trap finish EXIT

if [ "$#" -ne 4 ]; then
  echo "Usage $0 <remote_repository> <dist_code_name> <swift_version> <release> "
  exit 1
fi
RELEASE="$4"
echo "Repository $1 build $RELEASE contents:"
echo "------------------------"

#Adding the swift repository
echo deb $1 $RELEASE main >/etc/apt/sources.list.d/swift-distr.list
echo deb-src $1 $RELEASE main >>/etc/apt/sources.list.d/swift-distr.list

#Adding the ubuntu universe repository if it is not present.
if [[ "$1" != "*ubuntu*" ]]; then
  if ! grep -q '^deb http://archive.ubuntu.com/ubuntu '"$2"' universe' /etc/apt/sources.list /etc/apt/sources.list.d/*.list; then
    echo deb http://archive.ubuntu.com/ubuntu $2 universe >/etc/apt/sources.list.d/ubuntu-universe.list
  fi
fi

{
  apt-get remove -y swift-im
  apt-get autoremove -y
  apt-get update
  apt-get upgrade -y
  apt-get install -y apt-transport-https wget
  apt-get install -y dpkg-dev
  wget http://swift.im/keys/packages.key
  apt-key add packages.key
  apt-get update
} &>/dev/null

RETURN_VALUE=0
echo "Sources:"
{
  rm -rf swift-im*
  apt-get source swift-im=$3
} &>/dev/null
if [ "$?" -eq 0 ]; then
  echo "Sources files downloaded."
else
  echo "Sources files not present"
  RETURN_VALUE=$((RETURN_VALUE + ((1 << 0))))
fi

echo "Binary:"
{
  apt-cache show swift-im=$3*
} &>/dev/null
if [ "$?" -ne 0 ]; then
  echo "Swift package was not found in the repository."
  RETURN_VALUE=$((RETURN_VALUE + ((1 << 1))))
  exit $RETURN_VALUE
fi
echo "Installing binaries"
{
  apt-get install -y swift-im=$3*
} &>/dev/null
if [ "$?" -eq 0 ]; then
  echo "Installation was successful."
else
  echo "Installation failed"
  RETURN_VALUE=$((RETURN_VALUE + ((1 << 2))))
fi
echo "------------------------"
exit $RETURN_VALUE
