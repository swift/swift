#!/usr/bin/env bash

# This script installs the external dependencies to build Swift with Qt5.

SYSTEM_NAME=$(uname)

if [ "$SYSTEM_NAME" == "Linux" ] 
then
    # handle linux distributions
    SYSTEM_DISTRO=$(lsb_release -i -s)
    if [ "$SYSTEM_DISTRO" == "Debian" ]
    then
        sudo apt-get install pkg-config libssl-dev qt5-default libqt5x11extras5-dev libqt5webkit5-dev qtmultimedia5-dev qttools5-dev-tools qt5-image-formats-plugins libqt5svg5-dev libminiupnpc-dev libnatpmp-dev libhunspell-dev
    elif [ "$SYSTEM_DISTRO" == "Ubuntu" ]
    then
        sudo apt-get install pkg-config libssl-dev qt5-default libqt5x11extras5-dev libqt5webkit5-dev qtmultimedia5-dev qttools5-dev-tools qt5-image-formats-plugins libqt5svg5-dev libminiupnpc-dev libnatpmp-dev libhunspell-dev
    elif [ "$SYSTEM_DISTRO" == "Arch" ]
    then
        sudo pacman -S qt5-base qt5-x11extras qt5-webkit qt5-multimedia qt5-tools qt5-svg hunspell
    elif [ "$SYSTEM_DISTRO" == "openSUSE project" ]
    then
        sudo zypper in pkg-config libopenssl-devel libQt5Core-devel libQt5WebKit5-devel libQt5WebKitWidgets-devel libqt5-qtmultimedia-devel libqt5-qtx11extras-devel libqt5-qttools-devel libQt5Gui-devel libQt5Network-devel libQt5DBus-devel libQt5Svg5 python-xml hunspell-devel
    elif [ "$SYSTEM_DISTRO" == "Fedora" ]
    then
        sudo dnf groups install "C Development Tools and Libraries"
        sudo dnf install openssl-devel qt5-qtbase-devel qt5-linguist qt5-qtwebkit-devel qt5-qtmultimedia-devel qt5-qtx11extras-devel qt5-qtsvg-devel hunspell-devel
    else
        echo "Unsupported Linux distribution."
    fi
else
    echo "Unsupported system."
fi
