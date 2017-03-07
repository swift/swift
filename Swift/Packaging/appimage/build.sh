#!/bin/bash
SIXTYFOUR=''
cd /swift
git pull --rebase
mkdir -p /build
#scl enable devtoolset-2 './scons max_jobs=1 SWIFT_INSTALLDIR=/build /build'
./scons max_jobs=1 SWIFT_INSTALLDIR=/build /build
cd /build
ldd bin/swift-im | grep '=>' | awk '{print $3}' | xargs -I '{}' cp -v -L '{}' ./lib/ || true
mkdir -p lib/qt5
cp -r /usr/lib${SIXTYFOUR}/qt5/plugins ./lib/qt5/plugins
cp -L /lib${SIXTYFOUR}/libudev.so.0 ./lib/
cp /usr/lib${SIXTYFOUR}/libQt5XcbQpa.so* ./lib/
cp -L /usr/local/gcc-4.9.4/lib/libstdc++.so.6 ./lib/
mkdir -p /appstuff
cd /appstuff
bash -ex /Recipe /swift.yml
cd /appstuff/Swift/Swift.AppDir
rm AppRun
wget -c https://github.com/probonopd/AppImageKit/releases/download/6/AppRun_6-i686 -O AppRun
chmod +x AppRun
cd ..
rm appimagetool
wget -c https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-i686.AppImage -O appimagetool
chmod +x appimagetool
./appimagetool Swift.AppDir Swift-i686.AppImage
