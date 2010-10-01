/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "PresenceOracle.h"

#include <boost/bind.hpp>

#include "Swiften/Client/StanzaChannel.h"

namespace Swift {

PresenceOracle::PresenceOracle(StanzaChannel* stanzaChannel) {
	stanzaChannel_ = stanzaChannel;
	stanzaChannel_->onPresenceReceived.connect(boost::bind(&PresenceOracle::handleIncomingPresence, this, _1));
	stanzaChannel_->onAvailableChanged.connect(boost::bind(&PresenceOracle::handleStanzaChannelAvailableChanged, this, _1));
}

PresenceOracle::~PresenceOracle() {
	stanzaChannel_->onPresenceReceived.disconnect(boost::bind(&PresenceOracle::handleIncomingPresence, this, _1));
	stanzaChannel_->onAvailableChanged.disconnect(boost::bind(&PresenceOracle::handleStanzaChannelAvailableChanged, this, _1));
}

void PresenceOracle::handleStanzaChannelAvailableChanged(bool available) {
	if (available) {
		entries_.clear();
	}
}


void PresenceOracle::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	JID bareJID = JID(presence->getFrom().toBare());

	if (presence->getType() == Presence::Subscribe) {
		onPresenceSubscriptionRequest(bareJID, presence->getStatus());
	} 
	else {
		std::map<JID, boost::shared_ptr<Presence> > jidMap = entries_[bareJID];
		jidMap[presence->getFrom()] = presence;
		entries_[bareJID] = jidMap;
		onPresenceChange(presence);
	}
}

Presence::ref PresenceOracle::getLastPresence(const JID& jid) const {
	PresencesMap::const_iterator i = entries_.find(jid.toBare());
	if (i == entries_.end()) {
		return Presence::ref();
	}
	PresenceMap presenceMap = i->second;
	PresenceMap::const_iterator j = presenceMap.find(jid);
	if (j != presenceMap.end()) {
		return j->second;
	}
	else {
		return Presence::ref();
	}
}

Presence::ref PresenceOracle::getHighestPriorityPresence(const JID& bareJID) const {
	PresencesMap::const_iterator i = entries_.find(bareJID);
	if (i == entries_.end()) {
		return Presence::ref();
	}
	PresenceMap presenceMap = i->second;
	PresenceMap::const_iterator j = presenceMap.begin();
	Presence::ref highest;
	for (; j != presenceMap.end(); j++) {
		Presence::ref current = j->second;
		if (!highest
				|| current->getPriority() > highest->getPriority()
				|| (current->getPriority() == highest->getPriority()
						&& StatusShow::typeToAvailabilityOrdering(current->getShow()) > StatusShow::typeToAvailabilityOrdering(highest->getShow()))) {
			highest = current;
		}

	}
	return highest;
}

}
