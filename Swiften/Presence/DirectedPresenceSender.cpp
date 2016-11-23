/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Presence/DirectedPresenceSender.h>

namespace Swift {

DirectedPresenceSender::DirectedPresenceSender(PresenceSender* sender) : sender(sender) {
}

void DirectedPresenceSender::sendPresence(std::shared_ptr<Presence> presence) {
    if (!sender->isAvailable()) {
        return;
    }

    sender->sendPresence(presence);

    if (!presence->getTo().isValid()) {
        std::shared_ptr<Presence> presenceCopy(new Presence(*presence));
        for (const auto& jid : directedPresenceReceivers) {
            presenceCopy->setTo(jid);
            sender->sendPresence(presenceCopy);
        }

        lastSentUndirectedPresence = presence;
    }
}

/**
 * Gets the last broadcast presence, if none has been send the returned optional is not set.
 */
boost::optional<Presence::ref> DirectedPresenceSender::getLastSentUndirectedPresence() const {
    boost::optional<Presence::ref> presenceCopy =  lastSentUndirectedPresence ? boost::optional<Presence::ref>((*lastSentUndirectedPresence)->clone()) : boost::optional<Presence::ref>();
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
        if (lastSentUndirectedPresence && (*lastSentUndirectedPresence)->getType() == Presence::Available) {
            std::shared_ptr<Presence> presenceCopy((*lastSentUndirectedPresence)->clone());
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
        std::shared_ptr<Presence> presence(new Presence());
        presence->setType(Presence::Unavailable);
        presence->setTo(jid);
        sender->sendPresence(presence);
    }
}

bool DirectedPresenceSender::isAvailable() const {
    return sender->isAvailable();
}

}
