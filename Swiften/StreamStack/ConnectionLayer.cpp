/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/ConnectionLayer.h>

#include <boost/bind.hpp>

namespace Swift {

ConnectionLayer::ConnectionLayer(boost::shared_ptr<Connection> connection) : connection(connection) {
    connection->onDataRead.connect(boost::bind(&ConnectionLayer::handleDataRead, this, _1));
}

ConnectionLayer::~ConnectionLayer() {
    connection->onDataRead.disconnect(boost::bind(&ConnectionLayer::handleDataRead, this, _1));
}

void ConnectionLayer::handleDataRead(boost::shared_ptr<SafeByteArray> data) {
    writeDataToParentLayer(*data);
}


}
