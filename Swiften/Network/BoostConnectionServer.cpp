/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/BoostConnectionServer.h"

#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

BoostConnectionServer::BoostConnectionServer(int port, boost::asio::io_service* ioService) : port_(port), ioService_(ioService), acceptor_(NULL) {
}


void BoostConnectionServer::start() {
	try {
		assert(!acceptor_);
		acceptor_ = new boost::asio::ip::tcp::acceptor(
				*ioService_, 
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_));
		acceptNextConnection();
	}
	catch (const boost::system::system_error& e) {
		if (e.code() == boost::asio::error::address_in_use) {
			MainEventLoop::postEvent(boost::bind(boost::ref(onStopped), Conflict), shared_from_this());
		}
		else {
			MainEventLoop::postEvent(boost::bind(boost::ref(onStopped), UnknownError), shared_from_this());
		}
	}
}


void BoostConnectionServer::stop() {
	stop(boost::optional<Error>());
}

void BoostConnectionServer::stop(boost::optional<Error> e) {
	if (acceptor_) {
		acceptor_->close();
		delete acceptor_;
		acceptor_ = NULL;
	}
	MainEventLoop::postEvent(boost::bind(boost::ref(onStopped), e), shared_from_this());
}

void BoostConnectionServer::acceptNextConnection() {
	BoostConnection::ref newConnection(BoostConnection::create(&acceptor_->io_service()));
	acceptor_->async_accept(newConnection->getSocket(), 
		boost::bind(&BoostConnectionServer::handleAccept, shared_from_this(), newConnection, boost::asio::placeholders::error));
}

void BoostConnectionServer::handleAccept(boost::shared_ptr<BoostConnection> newConnection, const boost::system::error_code& error) {
	if (error) {
		MainEventLoop::postEvent(
				boost::bind(
						&BoostConnectionServer::stop, shared_from_this(), UnknownError), 
				shared_from_this());
	}
	else {
		MainEventLoop::postEvent(
				boost::bind(boost::ref(onNewConnection), newConnection), 
				shared_from_this());
		newConnection->listen();
		acceptNextConnection();
	}
}

}
