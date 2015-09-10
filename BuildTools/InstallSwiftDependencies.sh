#!/usr/bin/env bash

# This script installs the external dependencies to build Swift with Qt5.

SYSTEM_NAME=$(uname)

if [ "$SYSTEM_NAME" == "Linux" ] 
then
	# handle linux distributions
	SYSTEM_DISTRO=$(lsb_release -i -s)
	if [ "$SYSTEM_DISTRO" == "Debian" ] 
	then
		sudo apt-get install pkg-config libssl-dev qt5-default libqt5x11extras5-dev libqt5webkit5-dev qtmultimedia5-dev qttools5-dev-tools
	elif [ "$SYSTEM_DISTRO" == "Arch" ] 
	then
		sudo pacman -S qt5-base qt5-x11extras qt5-webkit qt5-multimedia qt5-tools
	else
		echo "Unsupported Linux distribution."
	fi
else
	echo "Unspupported system."
fi
