/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Client/StanzaChannel.h"

namespace Swift {

PresenceSender::PresenceSender(StanzaChannel* channel) : channel(channel) {
}

void PresenceSender::sendPresence(boost::shared_ptr<Presence> presence) {
	if (!channel->isAvailable()) {
		return;
	}

	channel->sendPresence(presence);

	if (!presence->getTo().isValid()) {
		boost::shared_ptr<Presence> presenceCopy(new Presence(*presence));
		foreach(const JID& jid, directedPresenceReceivers) {
			presenceCopy->setTo(jid);
			channel->sendPresence(presenceCopy);
		}

		lastSentUndirectedPresence = presence;
	}
}

void PresenceSender::addDirectedPresenceReceiver(const JID& jid) {
	directedPresenceReceivers.insert(jid);
	if (channel->isAvailable()) {
		if (lastSentUndirectedPresence && lastSentUndirectedPresence->getType() == Presence::Available) {
			boost::shared_ptr<Presence> presenceCopy(new Presence(*lastSentUndirectedPresence));
			presenceCopy->setTo(jid);
			channel->sendPresence(presenceCopy);
		}
	}
}

void PresenceSender::removeDirectedPresenceReceiver(const JID& jid) {
	directedPresenceReceivers.erase(jid);
	if (channel->isAvailable()) {
		boost::shared_ptr<Presence> presence(new Presence());
		presence->setType(Presence::Unavailable);
		presence->setTo(jid);
		channel->sendPresence(presence);
	}
}

}
