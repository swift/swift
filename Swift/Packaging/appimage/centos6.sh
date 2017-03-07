#!/bin/bash
yum -y install epel-release wget
# devtools turns out not to work, but it's a step I did so I'm recording it.
wget http://people.centos.org/tru/devtools-2/devtools-2.repo -O /etc/yum.repos.d/devtools-2.repo && yum -y upgrade
yum -y install devtoolset-2-gcc devtoolset-2-binutils devtoolset-2-gcc-c++ openssl-devel qt5-qtbase-devel qt5-qttools-libs-designercomponents qt5-qttools-devel qt5-qtwebkit-devel qt5-qtsvg-devel qt5-qttools-devel qt5-qttools-static qt5-qtmultimedia-devel qt5-qtwebchannel-devel qt5-qtimageformats-devel git patch qt5-qtmultimedia-devel qt5-qtx11extras-devel libtool fuse-libs libarchive desktop-file-utils gcc-c++
cd /
wget https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-i686.AppImage
chmod u+rx /appimagetool-i686.AppImage
wget https://raw.githubusercontent.com/probonopd/AppImages/master/recipes/meta/Recipe
yum -y install svn texinfo-tex flex zip libgcc.i686 glibc-devel.i686 
mkdir -p /usr/local/src
cd /usr/local/src
svn co svn://gcc.gnu.org/svn/gcc/tags/gcc_4_9_4_release/
cd gcc_4_9_4_release
./contrib/download_prerequisites
mkdir release_build
cd release_build
../configure --prefix=/usr/local/gcc-4.9.4
make
make install
echo "/usr/local/gcc-4.9.4/lib" > /etc/ld.so.conf.d/newgcc.conf
echo 'export PATH=/usr/local/gcc-4.9.4/bin:$PATH' >> /etc/profile
cd /
git clone https://github.com/swift/swift
# Run build.sh
