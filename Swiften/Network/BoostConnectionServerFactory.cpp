/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostConnectionServerFactory.h>

#include <Swiften/Network/BoostConnectionServer.h>

namespace Swift {

BoostConnectionServerFactory::BoostConnectionServerFactory(std::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : ioService(ioService), eventLoop(eventLoop) {
}

std::shared_ptr<ConnectionServer> BoostConnectionServerFactory::createConnectionServer(unsigned short port) {
    return BoostConnectionServer::create(port, ioService, eventLoop);
}

std::shared_ptr<ConnectionServer> BoostConnectionServerFactory::createConnectionServer(const Swift::HostAddress &hostAddress, unsigned short port) {
    return BoostConnectionServer::create(hostAddress, port, ioService, eventLoop);
}

}
