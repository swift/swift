FROM __DISTRO__
MAINTAINER packages@swift.im

ENV DEBIAN_VERSION=__DEBIAN_VERSION__

RUN \
  apt-get -qq update && \
  apt-get install -y lintian devscripts scons build-essential dh-make pkg-config libssl-dev qt5-default libqt5x11extras5-dev libqt5webkit5-dev qtmultimedia5-dev qttools5-dev-tools qt5-image-formats-plugins libqt5svg5-dev libminiupnpc-dev libnatpmp-dev libhunspell-dev libxml2-dev libxss-dev libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-regex-dev libboost-signals-dev libboost-system-dev libboost-thread-dev libboost-date-time-dev libidn11-dev  docbook-xsl docbook-xml xsltproc libxml2-utils libsqlite3-dev zlib1g-dev help2man && \
  apt-get clean

RUN mkdir /swift-packages
COPY * /swift-packages/

CMD \
  gpg --allow-secret-key-import --import /gpg-keys/*.asc && \
  cd /swift-packages  && \
  tar xf swift-im_${DEBIAN_VERSION}*.orig.tar.gz && \
  tar xf swift-im_${DEBIAN_VERSION}*.debian.tar.xz -C /swift-packages/swift-im-${DEBIAN_VERSION}/ && \
  cd swift-im-${DEBIAN_VERSION} && \
  debuild -i -I -b --lintian-opts --profile debian --pedantic && \
  cd /swift-packages/ && \
  rm -rf /swift-packages/swift-im-${DEBIAN_VERSION}/ && \
  rm -f /swift-packages/swift-im_${DEBIAN_VERSION}*.orig.tar.gz && \
  rm -f /swift-packages/swift-im_${DEBIAN_VERSION}*.debian.tar.xz && \
  rm -f /swift-packages/swift-im_${DEBIAN_VERSION}*.dsc && \
  rm -f /swift-packages/swift-im_${DEBIAN_VERSION}*_source.* && \
  mv * /out/
