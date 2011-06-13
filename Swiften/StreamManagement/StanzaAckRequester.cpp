/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StreamManagement/StanzaAckRequester.h>

#include <boost/numeric/conversion/cast.hpp>
#include <iostream>

#include <Swiften/Elements/Message.h>

namespace Swift {

static const unsigned int MAX_HANDLED_STANZA_COUNT = boost::numeric_cast<unsigned int>((1ULL<<32) - 1);

StanzaAckRequester::StanzaAckRequester() : lastHandledStanzasCount(0) {

}

void StanzaAckRequester::handleStanzaSent(boost::shared_ptr<Stanza> stanza) {
	unackedStanzas.push_back(stanza);
	if (boost::dynamic_pointer_cast<Message>(stanza)) {
		onRequestAck();
	}
}

void StanzaAckRequester::handleAckReceived(unsigned int handledStanzasCount) {
	unsigned int i = lastHandledStanzasCount;
	while (i != handledStanzasCount) {
		if (unackedStanzas.empty()) {
			std::cerr << "Warning: Server acked more stanzas than we sent" << std::endl;
			break;
		}
		boost::shared_ptr<Stanza> ackedStanza = unackedStanzas.front();
		unackedStanzas.pop_front();
		onStanzaAcked(ackedStanza);
		i = (i == MAX_HANDLED_STANZA_COUNT ? 0 : i + 1);
	}
	lastHandledStanzasCount = handledStanzasCount;
}

}
