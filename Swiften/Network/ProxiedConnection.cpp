/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/ProxiedConnection.h>

#include <boost/bind.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>

using namespace Swift;

ProxiedConnection::ProxiedConnection(
        DomainNameResolver* resolver,
        ConnectionFactory* connectionFactory,
        TimerFactory* timerFactory,
        const std::string& proxyHost,
        int proxyPort) :
            resolver_(resolver),
            connectionFactory_(connectionFactory),
            timerFactory_(timerFactory),
            proxyHost_(proxyHost),
            proxyPort_(proxyPort),
            server_(HostAddressPort(HostAddress::fromString("0.0.0.0").get(), 0)) {
    connected_ = false;
}

ProxiedConnection::~ProxiedConnection() {
    cancelConnector();
    if (connection_) {
        connection_->onDataRead.disconnect(boost::bind(&ProxiedConnection::handleDataRead, shared_from_this(), _1));
        connection_->onDisconnected.disconnect(boost::bind(&ProxiedConnection::handleDisconnected, shared_from_this(), _1));
    }
    if (connected_) {
        SWIFT_LOG(warning) << "Connection was still established." << std::endl;
    }
}

void ProxiedConnection::cancelConnector() {
    if (connector_) {
        connector_->onConnectFinished.disconnect(boost::bind(&ProxiedConnection::handleConnectFinished, shared_from_this(), _1));
        connector_->stop();
        connector_.reset();
    }
}

void ProxiedConnection::connect(const HostAddressPort& server) {
    server_ = server;

    connector_ = Connector::create(proxyHost_, proxyPort_, boost::optional<std::string>(), resolver_, connectionFactory_, timerFactory_);
    connector_->onConnectFinished.connect(boost::bind(&ProxiedConnection::handleConnectFinished, shared_from_this(), _1));
    connector_->start();
}

void ProxiedConnection::listen() {
    assert(false);
    connection_->listen();
}

void ProxiedConnection::disconnect() {
    cancelConnector();
    connected_ = false;
    if (connection_) {
        connection_->disconnect();
    }
}

void ProxiedConnection::handleDisconnected(const boost::optional<Error>& error) {
    onDisconnected(error);
}

void ProxiedConnection::write(const SafeByteArray& data) {
    connection_->write(data);
}

void ProxiedConnection::handleConnectFinished(Connection::ref connection) {
    cancelConnector();
    if (connection) {
        connection_ = connection;
        connection_->onDataRead.connect(boost::bind(&ProxiedConnection::handleDataRead, shared_from_this(), _1));
        connection_->onDisconnected.connect(boost::bind(&ProxiedConnection::handleDisconnected, shared_from_this(), _1));

        initializeProxy();
    }
    else {
        onConnectFinished(true);
    }
}

void ProxiedConnection::handleDataRead(std::shared_ptr<SafeByteArray> data) {
    if (!connected_) {
        handleProxyInitializeData(data);
    }
    else {
        onDataRead(data);
    }
}

HostAddressPort ProxiedConnection::getLocalAddress() const {
    return connection_->getLocalAddress();
}

HostAddressPort ProxiedConnection::getRemoteAddress() const {
    return connection_->getRemoteAddress();
}

void ProxiedConnection::setProxyInitializeFinished(bool success) {
    connected_ = success;
    if (!success) {
        disconnect();
    }
    onConnectFinished(!success);
}

void ProxiedConnection::reconnect() {
    if (connected_) {
        connection_->onDataRead.disconnect(boost::bind(&ProxiedConnection::handleDataRead, shared_from_this(), _1));
        connection_->onDisconnected.disconnect(boost::bind(&ProxiedConnection::handleDisconnected, shared_from_this(), _1));
        connection_->disconnect();
    }
    connect(server_);
}
