/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/TLSConnection.h>

#include <boost/bind.hpp>

#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {

TLSConnection::TLSConnection(Connection::ref connection, TLSContextFactory* tlsFactory) : connection(connection) {
	context = tlsFactory->createTLSContext();
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
	delete context;
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
	onDataRead(boost::make_shared<SafeByteArray>(data));
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

void TLSConnection::handleRawDataRead(boost::shared_ptr<SafeByteArray> data) {
	context->handleDataFromNetwork(*data);
}

void TLSConnection::handleRawDataWritten() {
	onDataWritten();
}

}
