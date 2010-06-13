#!/bin/bash

function test_versions() {
	versions=$@
	previous_version=
	for version in $versions; do
		if [ "$previous_version" ]; then
			debian_version=`./DebianizeVersion.py $version`
			debian_previous_version=`./DebianizeVersion.py $previous_version`
			dpkg --compare-versions $debian_version gt $debian_previous_version
			result=$?
			if [ "$result" != "0" ]; then
				echo "TEST FAILED: $debian_version > $debian_previous_version"
			fi
		fi
		previous_version=$version
	done
}

DEVELOPMENT_VERSIONS="
	swift-1.0beta1 
	swift-1.0beta1-dev3
	swift-1.0beta2 
	swift-1.0rc1
	swift-1.0rc1-dev2
	swift-1.0 
	swift-1.0-dev2
	swift-1.0-dev4 
	swift-2.0"
test_versions $DEVELOPMENT_VERSIONS

BETA_VERSIONS="
	swift-1.0beta1 
	swift-1.0beta2 
	swift-1.0rc1
	swift-1.0 
	swift-1.1rc1
	swift-1.1
	swift-2.0"
test_versions $BETA_VERSIONS

RELEASE_VERSIONS="
	swift-1.0 
	swift-1.1
	swift-2.0"
test_versions $RELEASE_VERSIONS
