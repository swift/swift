/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostConnectionServer.h>

#include <boost/asio/ip/v6_only.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {

BoostConnectionServer::BoostConnectionServer(unsigned short port, std::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : port_(port), ioService_(ioService), eventLoop(eventLoop), acceptor_(nullptr) {
}

BoostConnectionServer::BoostConnectionServer(const HostAddress &address, unsigned short port, std::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) : address_(address), port_(port), ioService_(ioService), eventLoop(eventLoop), acceptor_(nullptr) {
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
        boost::asio::ip::tcp::endpoint endpoint;
        if (address_.isValid()) {
            endpoint = boost::asio::ip::tcp::endpoint(address_.getRawAddress(), port_);
        }
        else {
            endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port_);
        }
        acceptor_ = new boost::asio::ip::tcp::acceptor(*ioService_, endpoint);
        if (endpoint.protocol() ==  boost::asio::ip::tcp::v6()) {
            boost::system::error_code ec;
            acceptor_->set_option(boost::asio::ip::v6_only(false), ec);
            SWIFT_LOG_ASSERT(ec, warning) << "IPv4/IPv6 dual-stack support is not supported on this platform." << std::endl;
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
    catch (const boost::numeric::bad_numeric_cast&) {
        return UnknownError;
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
        acceptor_ = nullptr;
    }
    eventLoop->postEvent(boost::bind(boost::ref(onStopped), e), shared_from_this());
}

void BoostConnectionServer::acceptNextConnection() {
    BoostConnection::ref newConnection(BoostConnection::create(ioService_, eventLoop));
    acceptor_->async_accept(newConnection->getSocket(),
        boost::bind(&BoostConnectionServer::handleAccept, shared_from_this(), newConnection, boost::asio::placeholders::error));
}

void BoostConnectionServer::handleAccept(std::shared_ptr<BoostConnection> newConnection, const boost::system::error_code& error) {
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
