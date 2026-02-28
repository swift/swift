/*
 * Copyright (c) 2013-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/StanzaAck.h>

using namespace Swift;

StanzaAck::~StanzaAck() {
}

void StanzaAck::setHandledStanzasCount(unsigned int i) {
    handledStanzasCount = i;
    valid = true;
}
