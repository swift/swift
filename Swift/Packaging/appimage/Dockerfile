FROM centos:6

RUN yum -y install epel-release wget
RUN wget http://people.centos.org/tru/devtools-2/devtools-2.repo -O /etc/yum.repos.d/devtools-2.repo && yum -y upgrade
RUN yum -y install devtoolset-2-gcc devtoolset-2-binutils devtoolset-2-gcc-c++ openssl-devel qt5-qtbase-devel qt5-qttools-libs-designercomponents qt5-qttools-devel qt5-qtwebkit-devel qt5-qtsvg-devel qt5-qttools-devel qt5-qttools-static qt5-qtmultimedia-devel qt5-qtwebchannel-devel qt5-qtimageformats-devel git patch qt5-qtmultimedia-devel qt5-qtx11extras-devel libtool
RUN cd / && git clone https://github.com/swift/swift
#RUN cd / && git clone --depth 1 https://github.com/probonopd/AppImageKit.git
#RUN cd /AppImageKit && scl enable devtoolset-2 ./build.sh
RUN cd / && wget https://github.com/probonopd/AppImageKit/releases/download/continuous/appimaged-x86_64.AppImage && chmod u+rx /appimaged-x86_64.AppImage
RUN yum -y install fuse-libs libarchive
RUN ln -s /usr/lib64/libarchive.so.2 /usr/lib64/libarchive.so.13
RUN cd / && wget https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage && chmod u+x appimagetool-x86_64.AppImage
RUN cd / && wget https://raw.githubusercontent.com/probonopd/AppImages/master/recipes/meta/Recipe
RUN yum -y install desktop-file-utils
ADD build.sh /build.sh
RUN chmod u+x /build.sh
#RUN cd /swift && scl enable devtoolset-2 './scons max_jobs=1'
ADD swift.yml /swift.yml
ADD config.py /swift/config.py
