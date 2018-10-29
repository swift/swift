/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamManagement/StanzaAckRequester.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/Message.h>

namespace Swift {

static const unsigned int MAX_HANDLED_STANZA_COUNT = static_cast<unsigned int>((1ULL<<32) - 1);

StanzaAckRequester::StanzaAckRequester() : lastHandledStanzasCount(0) {

}

void StanzaAckRequester::handleStanzaSent(std::shared_ptr<Stanza> stanza) {
    unackedStanzas.push_back(stanza);
    if (std::dynamic_pointer_cast<Message>(stanza)) {
        onRequestAck();
    }
}

void StanzaAckRequester::handleAckReceived(unsigned int handledStanzasCount) {
    unsigned int i = lastHandledStanzasCount;
    while (i != handledStanzasCount) {
        if (unackedStanzas.empty()) {
            SWIFT_LOG(warning) << "Server acked more stanzas than we sent" << std::endl;
            break;
        }
        std::shared_ptr<Stanza> ackedStanza = unackedStanzas.front();
        unackedStanzas.pop_front();
        onStanzaAcked(ackedStanza);
        i = (i == MAX_HANDLED_STANZA_COUNT ? 0 : i + 1);
    }
    lastHandledStanzasCount = handledStanzasCount;
}

}
