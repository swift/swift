/*
 * Copyright (c) 2011-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/TLSConnection.h>

#include <boost/bind.hpp>

#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {

TLSConnection::TLSConnection(Connection::ref connection, TLSContextFactory* tlsFactory, const TLSOptions& tlsOptions) : connection(connection) {
    context = tlsFactory->createTLSContext(tlsOptions);
    context->onDataForNetwork.connect(boost::bind(&TLSConnection::handleTLSDataForNetwork, this, _1));
    context->onDataForApplication.connect(boost::bind(&TLSConnection::handleTLSDataForApplication, this, _1));
    context->onConnected.connect(boost::bind(&TLSConnection::handleTLSConnectFinished, this, false));
    context->onError.connect(boost::bind(&TLSConnection::handleTLSConnectFinished, this, true));

    connection->onConnectFinished.connect(boost::bind(&TLSConnection::handleRawConnectFinished, this, _1));
    connection->onDataRead.connect(boost::bind(&TLSConnection::handleRawDataRead, this, _1));
    connection->onDataWritten.connect(boost::bind(&TLSConnection::handleRawDataWritten, this));
    connection->onDisconnected.connect(boost::bind(&TLSConnection::handleRawDisconnected, this, _1));
}

TLSConnection::~TLSConnection() {
    connection->onConnectFinished.disconnect(boost::bind(&TLSConnection::handleRawConnectFinished, this, _1));
    connection->onDataRead.disconnect(boost::bind(&TLSConnection::handleRawDataRead, this, _1));
    connection->onDataWritten.disconnect(boost::bind(&TLSConnection::handleRawDataWritten, this));
    connection->onDisconnected.disconnect(boost::bind(&TLSConnection::handleRawDisconnected, this, _1));
}

void TLSConnection::handleTLSConnectFinished(bool error) {
    onConnectFinished(error);
    if (error) {
        disconnect();
    }
}

void TLSConnection::handleTLSDataForNetwork(const SafeByteArray& data) {
    connection->write(data);
}

void TLSConnection::handleTLSDataForApplication(const SafeByteArray& data) {
    onDataRead(std::make_shared<SafeByteArray>(data));
}

void TLSConnection::connect(const HostAddressPort& address) {
    connection->connect(address);
}

void TLSConnection::disconnect() {
    connection->disconnect();
}

void TLSConnection::write(const SafeByteArray& data) {
    context->handleDataFromApplication(data);
}

HostAddressPort TLSConnection::getLocalAddress() const {
    return connection->getLocalAddress();
}

HostAddressPort TLSConnection::getRemoteAddress() const {
    return connection->getRemoteAddress();
}

TLSContext* TLSConnection::getTLSContext() const {
    return context.get();
}

void TLSConnection::handleRawConnectFinished(bool error) {
    connection->onConnectFinished.disconnect(boost::bind(&TLSConnection::handleRawConnectFinished, this, _1));
    if (error) {
        onConnectFinished(true);
    }
    else {
        context->connect();
    }
}

void TLSConnection::handleRawDisconnected(const boost::optional<Error>& error) {
    onDisconnected(error);
}

void TLSConnection::handleRawDataRead(std::shared_ptr<SafeByteArray> data) {
    context->handleDataFromNetwork(*data);
}

void TLSConnection::handleRawDataWritten() {
    onDataWritten();
}

}
