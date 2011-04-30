/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/ClientSessionStanzaChannel.h>

#include <boost/bind.hpp>
#include <iostream>

namespace Swift {

void ClientSessionStanzaChannel::setSession(boost::shared_ptr<ClientSession> session) {
	assert(!this->session);
	this->session = session;
	session->onInitialized.connect(boost::bind(&ClientSessionStanzaChannel::handleSessionInitialized, this));
	session->onFinished.connect(boost::bind(&ClientSessionStanzaChannel::handleSessionFinished, this, _1));
	session->onStanzaReceived.connect(boost::bind(&ClientSessionStanzaChannel::handleStanza, this, _1));
	session->onStanzaAcked.connect(boost::bind(&ClientSessionStanzaChannel::handleStanzaAcked, this, _1));
}

void ClientSessionStanzaChannel::sendIQ(boost::shared_ptr<IQ> iq) {
	send(iq);
}

void ClientSessionStanzaChannel::sendMessage(boost::shared_ptr<Message> message) {
	send(message);
}

void ClientSessionStanzaChannel::sendPresence(boost::shared_ptr<Presence> presence) {
	send(presence);
}

std::string ClientSessionStanzaChannel::getNewIQID() {
	return idGenerator.generateID();
}

void ClientSessionStanzaChannel::send(boost::shared_ptr<Stanza> stanza) {
	if (!isAvailable()) {
		std::cerr << "Warning: Client: Trying to send a stanza while disconnected." << std::endl;
		return;
	}
	session->sendStanza(stanza);
}

void ClientSessionStanzaChannel::handleSessionFinished(boost::shared_ptr<Error>) {
	session->onFinished.disconnect(boost::bind(&ClientSessionStanzaChannel::handleSessionFinished, this, _1));
	session->onStanzaReceived.disconnect(boost::bind(&ClientSessionStanzaChannel::handleStanza, this, _1));
	session->onStanzaAcked.disconnect(boost::bind(&ClientSessionStanzaChannel::handleStanzaAcked, this, _1));
	session->onInitialized.disconnect(boost::bind(&ClientSessionStanzaChannel::handleSessionInitialized, this));
	session.reset();

	onAvailableChanged(false);
}

void ClientSessionStanzaChannel::handleStanza(boost::shared_ptr<Stanza> stanza) {
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


bool ClientSessionStanzaChannel::getStreamManagementEnabled() const {
	if (session) {
		return session->getStreamManagementEnabled();
	}
	return false;
}

void ClientSessionStanzaChannel::handleStanzaAcked(boost::shared_ptr<Stanza> stanza) {
	onStanzaAcked(stanza);
}


void ClientSessionStanzaChannel::handleSessionInitialized() {
	onAvailableChanged(true);
}

}
