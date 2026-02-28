FROM debian:stretch
VOLUME ["/gpg-keys"]
MAINTAINER packages@swift.im

RUN \
  apt-get -qq update && \
  apt-get install -y lintian devscripts scons build-essential pkg-config libssl-dev qt5-default libqt5x11extras5-dev libqt5webkit5-dev qtmultimedia5-dev qttools5-dev-tools qt5-image-formats-plugins libqt5svg5-dev libminiupnpc-dev libnatpmp-dev libhunspell-dev libxml2-dev libxss-dev libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-regex-dev libboost-signals-dev libboost-system-dev libboost-thread-dev libboost-date-time-dev libidn11-dev  docbook-xsl docbook-xml xsltproc libxml2-utils libsqlite3-dev zlib1g-dev help2man && \
  apt-get clean

COPY swift-src /swift-src

CMD \
  gpg --allow-secret-key-import --import /gpg-keys/*.asc && \
  cd /swift-src && \
  mv swift-im_*.orig.tar.gz ../ && \
  debuild -i -I -S --lintian-opts --pedantic && \
  mv /swift-im* /out/
