/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Presence/SubscriptionManager.h>

#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Client/StanzaChannel.h>

namespace Swift {

SubscriptionManager::SubscriptionManager(StanzaChannel* channel) : stanzaChannel(channel) {
	stanzaChannel->onPresenceReceived.connect(boost::bind(&SubscriptionManager::handleIncomingPresence, this, _1));
}

SubscriptionManager::~SubscriptionManager() {
	stanzaChannel->onPresenceReceived.disconnect(boost::bind(&SubscriptionManager::handleIncomingPresence, this, _1));
}

void SubscriptionManager::cancelSubscription(const JID& jid) {
	Presence::ref stanza(new Presence());
	stanza->setType(Presence::Unsubscribed);
	stanza->setTo(jid);
	stanzaChannel->sendPresence(stanza);
}

void SubscriptionManager::confirmSubscription(const JID& jid) {
	Presence::ref stanza(new Presence());
	stanza->setType(Presence::Subscribed);
	stanza->setTo(jid);
	stanzaChannel->sendPresence(stanza);
}


void SubscriptionManager::requestSubscription(const JID& jid) {
	Presence::ref stanza(new Presence());
	stanza->setType(Presence::Subscribe);
	stanza->setTo(jid);
	stanzaChannel->sendPresence(stanza);
}

void SubscriptionManager::handleIncomingPresence(Presence::ref presence) {
	JID bareJID(presence->getFrom().toBare());
	if (presence->getType() == Presence::Subscribe) {
		onPresenceSubscriptionRequest(bareJID, presence->getStatus(), presence);
	}
	else if (presence->getType() == Presence::Unsubscribe) {
		onPresenceSubscriptionRevoked(bareJID, presence->getStatus());
	}
}


}
