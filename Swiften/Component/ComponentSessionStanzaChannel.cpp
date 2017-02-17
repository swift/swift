/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Component/ComponentSessionStanzaChannel.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

ComponentSessionStanzaChannel::~ComponentSessionStanzaChannel() {
    if (session) {
        session->onInitialized.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleSessionInitialized, this));
        session->onFinished.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleSessionFinished, this, _1));
        session->onStanzaReceived.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleStanza, this, _1));
        session.reset();
    }
}

void ComponentSessionStanzaChannel::setSession(std::shared_ptr<ComponentSession> session) {
    assert(!this->session);
    this->session = session;
    session->onInitialized.connect(boost::bind(&ComponentSessionStanzaChannel::handleSessionInitialized, this));
    session->onFinished.connect(boost::bind(&ComponentSessionStanzaChannel::handleSessionFinished, this, _1));
    session->onStanzaReceived.connect(boost::bind(&ComponentSessionStanzaChannel::handleStanza, this, _1));
}

void ComponentSessionStanzaChannel::sendIQ(std::shared_ptr<IQ> iq) {
    send(iq);
}

void ComponentSessionStanzaChannel::sendMessage(std::shared_ptr<Message> message) {
    send(message);
}

void ComponentSessionStanzaChannel::sendPresence(std::shared_ptr<Presence> presence) {
    send(presence);
}

std::string ComponentSessionStanzaChannel::getNewIQID() {
    return idGenerator.generateID();
}

void ComponentSessionStanzaChannel::send(std::shared_ptr<Stanza> stanza) {
    if (!isAvailable()) {
        SWIFT_LOG(warning) << "Component: Trying to send a stanza while disconnected." << std::endl;
        return;
    }
    session->sendStanza(stanza);
}

void ComponentSessionStanzaChannel::handleSessionFinished(std::shared_ptr<Error>) {
    session->onFinished.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleSessionFinished, this, _1));
    session->onStanzaReceived.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleStanza, this, _1));
    session->onInitialized.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleSessionInitialized, this));
    session.reset();

    onAvailableChanged(false);
}

void ComponentSessionStanzaChannel::handleStanza(std::shared_ptr<Stanza> stanza) {
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

void ComponentSessionStanzaChannel::handleSessionInitialized() {
    onAvailableChanged(true);
}

}
