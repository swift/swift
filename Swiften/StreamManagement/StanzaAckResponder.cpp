/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamManagement/StanzaAckResponder.h>

namespace Swift {

static const unsigned int MAX_HANDLED_STANZA_COUNT = static_cast<unsigned int>((1ULL << 32) - 1);

StanzaAckResponder::StanzaAckResponder() : handledStanzasCount(0) {
}

void StanzaAckResponder::handleStanzaReceived() {
    handledStanzasCount = (handledStanzasCount == MAX_HANDLED_STANZA_COUNT ? 0 : handledStanzasCount + 1);
}

void StanzaAckResponder::handleAckRequestReceived() {
    onAck(handledStanzasCount);
}

}
