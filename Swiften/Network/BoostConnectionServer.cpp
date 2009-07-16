#include "Swiften/Network/BoostConnectionServer.h"

#include <boost/bind.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

BoostConnectionServer::BoostConnectionServer(int port, boost::asio::io_service* ioService) : acceptor_(*ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
}


void BoostConnectionServer::start() {
	acceptNextConnection();
}

void BoostConnectionServer::acceptNextConnection() {
	boost::shared_ptr<BoostConnection> newConnection(new BoostConnection(&acceptor_.io_service()));
	acceptor_.async_accept(newConnection->getSocket(), 
		boost::bind(&BoostConnectionServer::handleAccept, shared_from_this(), newConnection, boost::asio::placeholders::error));
}

void BoostConnectionServer::handleAccept(boost::shared_ptr<BoostConnection> newConnection, const boost::system::error_code& error) {
	if (!error) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onNewConnection), newConnection), shared_from_this());
		newConnection->listen();
		acceptNextConnection();
	}
}

}
