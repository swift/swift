/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/ClientSessionStanzaChannel.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

ClientSessionStanzaChannel::~ClientSessionStanzaChannel() {
    if (session) {
        session->onFinished.disconnect(boost::bind(&ClientSessionStanzaChannel::handleSessionFinished, this, _1));
        session->onStanzaReceived.disconnect(boost::bind(&ClientSessionStanzaChannel::handleStanza, this, _1));
        session->onStanzaAcked.disconnect(boost::bind(&ClientSessionStanzaChannel::handleStanzaAcked, this, _1));
        session->onInitialized.disconnect(boost::bind(&ClientSessionStanzaChannel::handleSessionInitialized, this));
        session.reset();
    }
}

void ClientSessionStanzaChannel::setSession(std::shared_ptr<ClientSession> session) {
    assert(!this->session);
    this->session = session;
    session->onInitialized.connect(boost::bind(&ClientSessionStanzaChannel::handleSessionInitialized, this));
    session->onFinished.connect(boost::bind(&ClientSessionStanzaChannel::handleSessionFinished, this, _1));
    session->onStanzaReceived.connect(boost::bind(&ClientSessionStanzaChannel::handleStanza, this, _1));
    session->onStanzaAcked.connect(boost::bind(&ClientSessionStanzaChannel::handleStanzaAcked, this, _1));
}

void ClientSessionStanzaChannel::sendIQ(std::shared_ptr<IQ> iq) {
    send(iq);
}

void ClientSessionStanzaChannel::sendMessage(std::shared_ptr<Message> message) {
    send(message);
}

void ClientSessionStanzaChannel::sendPresence(std::shared_ptr<Presence> presence) {
    send(presence);
}

std::string ClientSessionStanzaChannel::getNewIQID() {
    return idGenerator.generateID();
}

void ClientSessionStanzaChannel::send(std::shared_ptr<Stanza> stanza) {
    if (!isAvailable()) {
        SWIFT_LOG(warning) << "Client: Trying to send a stanza while disconnected." << std::endl;
        return;
    }
    session->sendStanza(stanza);
}

void ClientSessionStanzaChannel::handleSessionFinished(std::shared_ptr<Error>) {
    session->onFinished.disconnect(boost::bind(&ClientSessionStanzaChannel::handleSessionFinished, this, _1));
    session->onStanzaReceived.disconnect(boost::bind(&ClientSessionStanzaChannel::handleStanza, this, _1));
    session->onStanzaAcked.disconnect(boost::bind(&ClientSessionStanzaChannel::handleStanzaAcked, this, _1));
    session->onInitialized.disconnect(boost::bind(&ClientSessionStanzaChannel::handleSessionInitialized, this));
    session.reset();

    onAvailableChanged(false);
}

void ClientSessionStanzaChannel::handleStanza(std::shared_ptr<Stanza> stanza) {
    std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(stanza);
    if (message) {
        onMessageReceived(message);
        return;
    }

    std::shared_ptr<Presence> presence = std::dynamic_pointer_cast<Presence>(stanza);
    if (presence) {
        onPresenceReceived(presence);
        return;
    }

    std::shared_ptr<IQ> iq = std::dynamic_pointer_cast<IQ>(stanza);
    if (iq) {
        onIQReceived(iq);
        return;
    }
}


bool ClientSessionStanzaChannel::getStreamManagementEnabled() const {
    if (session) {
        return session->getStreamManagementEnabled();
    }
    return false;
}

std::vector<Certificate::ref> ClientSessionStanzaChannel::getPeerCertificateChain() const {
    if (session) {
        return session->getPeerCertificateChain();
    }
    return std::vector<Certificate::ref>();
}

void ClientSessionStanzaChannel::handleStanzaAcked(std::shared_ptr<Stanza> stanza) {
    onStanzaAcked(stanza);
}


void ClientSessionStanzaChannel::handleSessionInitialized() {
    onAvailableChanged(true);
}

}
