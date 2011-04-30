/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/ComponentSessionStanzaChannel.h>

#include <boost/bind.hpp>
#include <iostream>

namespace Swift {

void ComponentSessionStanzaChannel::setSession(boost::shared_ptr<ComponentSession> session) {
	assert(!this->session);
	this->session = session;
	session->onInitialized.connect(boost::bind(&ComponentSessionStanzaChannel::handleSessionInitialized, this));
	session->onFinished.connect(boost::bind(&ComponentSessionStanzaChannel::handleSessionFinished, this, _1));
	session->onStanzaReceived.connect(boost::bind(&ComponentSessionStanzaChannel::handleStanza, this, _1));
}

void ComponentSessionStanzaChannel::sendIQ(boost::shared_ptr<IQ> iq) {
	send(iq);
}

void ComponentSessionStanzaChannel::sendMessage(boost::shared_ptr<Message> message) {
	send(message);
}

void ComponentSessionStanzaChannel::sendPresence(boost::shared_ptr<Presence> presence) {
	send(presence);
}

std::string ComponentSessionStanzaChannel::getNewIQID() {
	return idGenerator.generateID();
}

void ComponentSessionStanzaChannel::send(boost::shared_ptr<Stanza> stanza) {
	if (!isAvailable()) {
		std::cerr << "Warning: Component: Trying to send a stanza while disconnected." << std::endl;
		return;
	}
	session->sendStanza(stanza);
}

void ComponentSessionStanzaChannel::handleSessionFinished(boost::shared_ptr<Error>) {
	session->onFinished.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleSessionFinished, this, _1));
	session->onStanzaReceived.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleStanza, this, _1));
	session->onInitialized.disconnect(boost::bind(&ComponentSessionStanzaChannel::handleSessionInitialized, this));
	session.reset();

	onAvailableChanged(false);
}

void ComponentSessionStanzaChannel::handleStanza(boost::shared_ptr<Stanza> stanza) {
	boost::shared_ptr<Message> message = boost::dynamic_pointer_cast<Message>(stanza);
	if (message) {
		onMessageReceived(message);
		return;
	}

	boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(stanza);
	if (presence) {
		onPresenceReceived(presence);
		return;
	}

	boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(stanza);
	if (iq) {
		onIQReceived(iq);
		return;
	}
}

void ComponentSessionStanzaChannel::handleSessionInitialized() {
	onAvailableChanged(true);
}

}
