/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostConnectionFactory.h>

#include <Swiften/Network/BoostConnection.h>

namespace Swift {

BoostConnectionFactory::BoostConnectionFactory(std::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : ioService(ioService), eventLoop(eventLoop) {
}

std::shared_ptr<Connection> BoostConnectionFactory::createConnection() {
    return BoostConnection::create(ioService, eventLoop);
}

}
