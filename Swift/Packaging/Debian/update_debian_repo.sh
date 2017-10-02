#!/usr/bin/env bash

# This script uses reprepro to create or update an apt repository with new packages
# from an incoming directory for multiple distributions and update channels.
#
# Release Channels: stable (apt name release), testing (apt name beta), development (apt name development)
# Packages: swift-im, libswiften, libswiften-dev

# fail on error
set -e
shopt -s nullglob

# configuration via environment variables
INCOMING_FOLDER="${SWIFT_APT_INCOMING_FOLDER:-error}"
if [ "$INCOMING_FOLDER" = "error" ];
then
	{ echo >&2 "Please set SWIFT_APT_INCOMING_FOLDER to the folder where new packages are read from."; exit 1; }
fi

APT_REPO_ROOT="${SWIFT_APT_REPO_ROOT:-error}"
if [ "$APT_REPO_ROOT" = "error" ];
then
	{ echo >&2 "Please set SWIFT_APT_REPO_ROOT to the folder where the root of the apt repository should be created."; exit 1; }
fi

# Prepare temporary directory used by reprepro
APT_TEMP_DIR=/tmp/swift_reprepro_tmp
rm -rfd $APT_TEMP_DIR
mkdir -p $APT_TEMP_DIR

function write_reprepo_conf_distributions_to_file {
cat <<EOM >$1
Codename: beta
Components: main
Architectures: i386 amd64 source
SignWith: packages@swift.im
Contents: . .gz .bz2
DebIndices: Packages Release . .gz .bz2
DscIndices: Sources Release . .gz .bz2

Codename: development
Components: main
Architectures: i386 amd64 source
SignWith: packages@swift.im
Contents: . .gz .bz2
DebIndices: Packages Release . .gz .bz2
DscIndices: Sources Release . .gz .bz2

Codename: release
Components: main
Architectures: i386 amd64 source
SignWith: packages@swift.im
Contents: . .gz .bz2
DebIndices: Packages Release . .gz .bz2
DscIndices: Sources Release . .gz .bz2
EOM
}

function write_reprepo_conf_incoming_to_file {
cat <<EOM >$1
Name: swift
IncomingDir: $2
TempDir: $APT_TEMP_DIR
Allow: beta development release
Multiple: Yes
Cleanup: unused_files on_error
Permit: unused_files older_version
EOM
}

# check for existence of reprepro command
command -v reprepro >/dev/null 2>&1 || { echo >&2 "This script requires aptly but it's not installed. See https://www.aptly.info/ for further information. Aborting."; exit 1; }

mkdir -p $APT_REPO_ROOT

# distros
for full_distribution_path in $INCOMING_FOLDER/{debian,ubuntu}/*; do
	distro_version=`basename $full_distribution_path`
	distro_name=$(basename `dirname $full_distribution_path`)
	distro_reprepro_root=${APT_REPO_ROOT}/${distro_name}/${distro_version}

	# ensure reprepro diretctory for this distribution version is present
	if [ ! -d "$distro_preprepro_root" ]; then
		echo "Create distribution repository for $distro_name/$distro_version"
		mkdir -p $distro_reprepro_root
		mkdir -p ${distro_reprepro_root}/conf

		write_reprepo_conf_distributions_to_file "${distro_reprepro_root}/conf/distributions"
		write_reprepo_conf_incoming_to_file "${distro_reprepro_root}/conf/incoming" "$full_distribution_path"
	fi

	# This is workaround for https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=808558 
	# and https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=843402
	if [ "$distro_name/$distro_version" = "debian/sid" ]; then
		sed -i '/dbgsym/ d' $full_distribution_path/*.changes
	fi

	echo "Process incoming packages for $distro_name/$distro_version"
	#set +e
	reprepro -V -b ${distro_reprepro_root} processincoming swift
	#set -e
done
echo "Successfully finished updating apt repository."
