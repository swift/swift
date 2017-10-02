/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Presence/StanzaChannelPresenceSender.h>

#include <Swiften/Client/StanzaChannel.h>

namespace Swift {

StanzaChannelPresenceSender::StanzaChannelPresenceSender(StanzaChannel* channel) : channel(channel) {
}

void StanzaChannelPresenceSender::sendPresence(Presence::ref presence) {
    channel->sendPresence(presence);
}

bool StanzaChannelPresenceSender::isAvailable() const {
    return channel->isAvailable();
}

}
