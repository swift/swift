/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StreamManagement/StanzaAckResponder.h>

#include <boost/numeric/conversion/cast.hpp>

namespace Swift {

static const unsigned int MAX_HANDLED_STANZA_COUNT = boost::numeric_cast<unsigned int>((1ULL<<32) - 1);

StanzaAckResponder::StanzaAckResponder() : handledStanzasCount(0) {
}

void StanzaAckResponder::handleStanzaReceived() {
	handledStanzasCount = (handledStanzasCount == MAX_HANDLED_STANZA_COUNT ? 0 : handledStanzasCount + 1);
}

void StanzaAckResponder::handleAckRequestReceived() {
	onAck(handledStanzasCount);
}

}
