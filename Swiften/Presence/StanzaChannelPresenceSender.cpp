/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
