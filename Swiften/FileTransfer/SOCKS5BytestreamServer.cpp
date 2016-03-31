/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>
#include <Swiften/StringCodecs/Hexify.h>

namespace Swift {

SOCKS5BytestreamServer::SOCKS5BytestreamServer(
        boost::shared_ptr<ConnectionServer> connectionServer,
        SOCKS5BytestreamRegistry* registry) :
            connectionServer(connectionServer),
            registry(registry) {
}

void SOCKS5BytestreamServer::start() {
    connectionServer->onNewConnection.connect(boost::bind(&SOCKS5BytestreamServer::handleNewConnection, this, _1));
}

void SOCKS5BytestreamServer::stop() {
    connectionServer->onNewConnection.disconnect(boost::bind(&SOCKS5BytestreamServer::handleNewConnection, this, _1));
    foreach (boost::shared_ptr<SOCKS5BytestreamServerSession> session, sessions) {
        session->onFinished.disconnect(boost::bind(&SOCKS5BytestreamServer::handleSessionFinished, this, session));
        session->stop();
    }
    sessions.clear();
}

void SOCKS5BytestreamServer::handleNewConnection(boost::shared_ptr<Connection> connection) {
    boost::shared_ptr<SOCKS5BytestreamServerSession> session =
        boost::make_shared<SOCKS5BytestreamServerSession>(connection, registry);
    session->onFinished.connect(boost::bind(&SOCKS5BytestreamServer::handleSessionFinished, this, session));
    sessions.push_back(session);
    session->start();
}

HostAddressPort SOCKS5BytestreamServer::getAddressPort() const {
    return connectionServer->getAddressPort();
}

std::vector< boost::shared_ptr<SOCKS5BytestreamServerSession> > SOCKS5BytestreamServer::getSessions(
        const std::string& streamID) const {
    std::vector< boost::shared_ptr<SOCKS5BytestreamServerSession> > result;
    foreach (boost::shared_ptr<SOCKS5BytestreamServerSession> session, sessions) {
        if (session->getStreamID() == streamID) {
            result.push_back(session);
        }
    }
    return result;
}

void SOCKS5BytestreamServer::handleSessionFinished(boost::shared_ptr<SOCKS5BytestreamServerSession> session) {
    sessions.erase(std::remove(sessions.begin(), sessions.end(), session), sessions.end());
    session->onFinished.disconnect(boost::bind(&SOCKS5BytestreamServer::handleSessionFinished, this, session));
}

}

