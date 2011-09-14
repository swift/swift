/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Presence/PayloadAddingPresenceSender.h>

namespace Swift {

PayloadAddingPresenceSender::PayloadAddingPresenceSender(PresenceSender* sender) : sender(sender) {
}

void PayloadAddingPresenceSender::sendPresence(Presence::ref presence) {
	if (presence->isAvailable()) {
		if (!presence->getTo().isValid()) {
			lastSentPresence = presence;
		}
	}
	else {
		lastSentPresence.reset();
	}
	if (payload) {
		Presence::ref sentPresence = Presence::create(presence);
		sentPresence->updatePayload(payload);
		sender->sendPresence(sentPresence);
	}
	else {
		sender->sendPresence(presence);
	}
}

bool PayloadAddingPresenceSender::isAvailable() const {
	return sender->isAvailable();
}

void PayloadAddingPresenceSender::setPayload(boost::shared_ptr<Payload> payload) {
	this->payload = payload;
	if (lastSentPresence) {
		sendPresence(lastSentPresence);
	}
}

void PayloadAddingPresenceSender::reset() {
	lastSentPresence.reset();
}

}
