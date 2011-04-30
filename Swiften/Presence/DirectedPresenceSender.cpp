/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

DirectedPresenceSender::DirectedPresenceSender(PresenceSender* sender) : sender(sender) {
}

void DirectedPresenceSender::sendPresence(boost::shared_ptr<Presence> presence) {
	if (!sender->isAvailable()) {
		return;
	}

	sender->sendPresence(presence);

	if (!presence->getTo().isValid()) {
		boost::shared_ptr<Presence> presenceCopy(new Presence(*presence));
		foreach(const JID& jid, directedPresenceReceivers) {
			presenceCopy->setTo(jid);
			sender->sendPresence(presenceCopy);
		}

		lastSentUndirectedPresence = presence;
	}
}

/**
 * Gets either the last broadcast presence, or an empty stanza if none has been sent.
 */
boost::shared_ptr<Presence> DirectedPresenceSender::getLastSentUndirectedPresence() {
	boost::shared_ptr<Presence> presenceCopy(lastSentUndirectedPresence ? new Presence(*lastSentUndirectedPresence) : new Presence());
	return presenceCopy;
}

/**
 * Send future broadcast presence also to this JID.
 * @param jid Non-roster JID to receive global presence updates.
 * @param sendPresence Also send the current global presence immediately.
 */
void DirectedPresenceSender::addDirectedPresenceReceiver(const JID& jid, SendPresence sendPresence) {
	directedPresenceReceivers.insert(jid);
	if (sendPresence == AndSendPresence && sender->isAvailable()) {
		if (lastSentUndirectedPresence && lastSentUndirectedPresence->getType() == Presence::Available) {
			boost::shared_ptr<Presence> presenceCopy(new Presence(*lastSentUndirectedPresence));
			presenceCopy->setTo(jid);
			sender->sendPresence(presenceCopy);
		}
	}
}

/**
 * Send future broadcast presence also to this JID.
 * @param jid Non-roster JID to stop receiving global presence updates.
 * @param sendPresence Also send presence type=unavailable immediately to jid.
 */
void DirectedPresenceSender::removeDirectedPresenceReceiver(const JID& jid, SendPresence sendPresence) {
	directedPresenceReceivers.erase(jid);
	if (sendPresence == AndSendPresence && sender->isAvailable()) {
		boost::shared_ptr<Presence> presence(new Presence());
		presence->setType(Presence::Unavailable);
		presence->setTo(jid);
		sender->sendPresence(presence);
	}
}

bool DirectedPresenceSender::isAvailable() const {
	return sender->isAvailable();
}

}
