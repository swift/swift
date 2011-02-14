/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/FileTransfer/SOCKS5BytestreamServer.h"

#include <boost/bind.hpp>

#include "Swiften/StringCodecs/Hexify.h"
#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/FileTransfer/SOCKS5BytestreamServerSession.h"

namespace Swift {

SOCKS5BytestreamServer::SOCKS5BytestreamServer(boost::shared_ptr<ConnectionServer> connectionServer) : connectionServer(connectionServer) {
}

void SOCKS5BytestreamServer::start() {
	connectionServer->onNewConnection.connect(boost::bind(&SOCKS5BytestreamServer::handleNewConnection, this, _1));
}

void SOCKS5BytestreamServer::stop() {
	connectionServer->onNewConnection.disconnect(boost::bind(&SOCKS5BytestreamServer::handleNewConnection, this, _1));
}

void SOCKS5BytestreamServer::addBytestream(const std::string& id, const JID& from, const JID& to, boost::shared_ptr<ReadBytestream> byteStream) {
	bytestreams.addBytestream(getSOCKSDestinationAddress(id, from, to), byteStream);
}

void SOCKS5BytestreamServer::removeBytestream(const std::string& id, const JID& from, const JID& to) {
	bytestreams.removeBytestream(getSOCKSDestinationAddress(id, from, to));
}

std::string SOCKS5BytestreamServer::getSOCKSDestinationAddress(const std::string& id, const JID& from, const JID& to) {
	return Hexify::hexify(SHA1::getHash(ByteArray(id + from.toString() + to.toString())));
}

void SOCKS5BytestreamServer::handleNewConnection(boost::shared_ptr<Connection> connection) {
	boost::shared_ptr<SOCKS5BytestreamServerSession> session(new SOCKS5BytestreamServerSession(connection, &bytestreams));
	sessions.push_back(session);
	session->start();
}

HostAddressPort SOCKS5BytestreamServer::getAddressPort() const {
	return connectionServer->getAddressPort();
}

}

