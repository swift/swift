/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "PresenceOracle.h"

#include <boost/bind.hpp>

#include <Swiften/Client/StanzaChannel.h>

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


void PresenceOracle::handleIncomingPresence(Presence::ref presence) {
	JID bareJID(presence->getFrom().toBare());
	if (presence->getType() == Presence::Subscribe) {
	}
	else {
		Presence::ref passedPresence = presence;
		if (presence->getType() == Presence::Unsubscribe) {
			/* 3921bis says that we don't follow up with an unavailable, so simulate this ourselves */
			passedPresence = Presence::ref(new Presence());
			passedPresence->setType(Presence::Unavailable);
			passedPresence->setFrom(bareJID);
			passedPresence->setStatus(presence->getStatus());
		}
		std::map<JID, boost::shared_ptr<Presence> > jidMap = entries_[bareJID];
		if (passedPresence->getFrom().isBare() && presence->getType() == Presence::Unavailable) {
			/* Have a bare-JID only presence of offline */
			jidMap.clear();
		} else if (passedPresence->getType() == Presence::Available) {
			/* Don't have a bare-JID only offline presence once there are available presences */
			jidMap.erase(bareJID);
		}
		if (passedPresence->getType() == Presence::Unavailable && jidMap.size() > 1) {
			jidMap.erase(passedPresence->getFrom());
		} else {
			jidMap[passedPresence->getFrom()] = passedPresence;
		}
		entries_[bareJID] = jidMap;
		onPresenceChange(passedPresence);
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

std::vector<Presence::ref> PresenceOracle::getAllPresence(const JID& bareJID) const {
	std::vector<Presence::ref> results;
	PresencesMap::const_iterator i = entries_.find(bareJID);
	if (i == entries_.end()) {
		return results;
	}
	PresenceMap presenceMap = i->second;
	PresenceMap::const_iterator j = presenceMap.begin();
	for (; j != presenceMap.end(); ++j) {
		Presence::ref current = j->second;
		results.push_back(current);
	}
	return results;
}

Presence::ref PresenceOracle::getHighestPriorityPresence(const JID& bareJID) const {
	PresencesMap::const_iterator i = entries_.find(bareJID);
	if (i == entries_.end()) {
		return Presence::ref();
	}
	PresenceMap presenceMap = i->second;
	PresenceMap::const_iterator j = presenceMap.begin();
	Presence::ref highest;
	for (; j != presenceMap.end(); ++j) {
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
