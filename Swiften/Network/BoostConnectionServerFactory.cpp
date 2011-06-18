/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/BoostConnectionServerFactory.h>
#include <Swiften/Network/BoostConnectionServer.h>

namespace Swift {

BoostConnectionServerFactory::BoostConnectionServerFactory(boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : ioService(ioService), eventLoop(eventLoop) {
}

boost::shared_ptr<ConnectionServer> BoostConnectionServerFactory::createConnectionServer(int port) {
	return BoostConnectionServer::create(port, ioService, eventLoop);
}

boost::shared_ptr<ConnectionServer> BoostConnectionServerFactory::createConnectionServer(const Swift::HostAddress &hostAddress, int port) {
	return BoostConnectionServer::create(hostAddress, port, ioService, eventLoop);
}

}
