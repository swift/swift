/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/BoostConnectionServer.h>

#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {

BoostConnectionServer::BoostConnectionServer(int port, boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : port_(port), ioService_(ioService), eventLoop(eventLoop), acceptor_(NULL) {
}

BoostConnectionServer::BoostConnectionServer(const HostAddress &address, int port, boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : address_(address), port_(port), ioService_(ioService), eventLoop(eventLoop), acceptor_(NULL) {
}

void BoostConnectionServer::start() {
	boost::optional<Error> error = tryStart();
	if (error) {
		eventLoop->postEvent(boost::bind(boost::ref(onStopped), *error), shared_from_this());
	}
}

boost::optional<BoostConnectionServer::Error> BoostConnectionServer::tryStart() {
	try {
		assert(!acceptor_);
		if (address_.isValid()) {
			acceptor_ = new boost::asio::ip::tcp::acceptor(
				*ioService_, 
				boost::asio::ip::tcp::endpoint(address_.getRawAddress(), boost::numeric_cast<unsigned short>(port_)));
		}
		else {
			acceptor_ = new boost::asio::ip::tcp::acceptor(
				*ioService_, 
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), boost::numeric_cast<unsigned short>(port_)));
		}
		acceptNextConnection();
	}
	catch (const boost::system::system_error& e) {
		if (e.code() == boost::asio::error::address_in_use) {
			return Conflict;
		}
		else {
			return UnknownError;
		}
	}
	return boost::optional<Error>();
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
	eventLoop->postEvent(boost::bind(boost::ref(onStopped), e), shared_from_this());
}

void BoostConnectionServer::acceptNextConnection() {
	BoostConnection::ref newConnection(BoostConnection::create(ioService_, eventLoop));
	acceptor_->async_accept(newConnection->getSocket(), 
		boost::bind(&BoostConnectionServer::handleAccept, shared_from_this(), newConnection, boost::asio::placeholders::error));
}

void BoostConnectionServer::handleAccept(boost::shared_ptr<BoostConnection> newConnection, const boost::system::error_code& error) {
	if (error) {
		eventLoop->postEvent(
				boost::bind(
						&BoostConnectionServer::stop, shared_from_this(), UnknownError), 
				shared_from_this());
	}
	else {
		eventLoop->postEvent(
				boost::bind(boost::ref(onNewConnection), newConnection), 
				shared_from_this());
		newConnection->listen();
		acceptNextConnection();
	}
}

HostAddressPort BoostConnectionServer::getAddressPort() const {
	if (acceptor_) {
		return HostAddressPort(acceptor_->local_endpoint());
	}
	else {
		return HostAddressPort();
	}
}

}
