/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Presence/PresenceOracle.h>

#include <queue>

#include <boost/bind.hpp>

#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/StatusShow.h>
#include <Swiften/Roster/XMPPRoster.h>

namespace Swift {

PresenceOracle::PresenceOracle(StanzaChannel* stanzaChannel, XMPPRoster* roster) : stanzaChannel_(stanzaChannel), xmppRoster_(roster) {
    stanzaChannel_->onPresenceReceived.connect(boost::bind(&PresenceOracle::handleIncomingPresence, this, _1));
    stanzaChannel_->onAvailableChanged.connect(boost::bind(&PresenceOracle::handleStanzaChannelAvailableChanged, this, _1));
    xmppRoster_->onJIDRemoved.connect(boost::bind(&PresenceOracle::handleJIDRemoved, this, _1));
}

PresenceOracle::~PresenceOracle() {
    stanzaChannel_->onPresenceReceived.disconnect(boost::bind(&PresenceOracle::handleIncomingPresence, this, _1));
    stanzaChannel_->onAvailableChanged.disconnect(boost::bind(&PresenceOracle::handleStanzaChannelAvailableChanged, this, _1));
    xmppRoster_->onJIDRemoved.disconnect(boost::bind(&PresenceOracle::handleJIDRemoved, this, _1));
}

void PresenceOracle::handleStanzaChannelAvailableChanged(bool available) {
    if (available) {
        entries_.clear();
    }
}

void PresenceOracle::handleIncomingPresence(Presence::ref presence) {
    JID bareJID(presence->getFrom().toBare());
    if (presence->getType() == Presence::Subscribe) {
    }
    else {
        Presence::ref passedPresence = presence;
        if (presence->getType() == Presence::Unsubscribe) {
            /* 3921bis says that we don't follow up with an unavailable, so simulate this ourselves */
            passedPresence = Presence::ref(new Presence());
            passedPresence->setType(Presence::Unavailable);
            passedPresence->setFrom(bareJID);
            passedPresence->setStatus(presence->getStatus());
        }
        PresenceMap jidMap = entries_[bareJID];
        if (passedPresence->getFrom().isBare() && presence->getType() == Presence::Unavailable) {
            /* Have a bare-JID only presence of offline */
            jidMap.clear();
        } else if (passedPresence->getType() == Presence::Available) {
            /* Don't have a bare-JID only offline presence once there are available presences */
            jidMap.erase(bareJID);
        }
        if (passedPresence->getType() == Presence::Unavailable && jidMap.size() > 1) {
            jidMap.erase(passedPresence->getFrom());
        } else {
            jidMap[passedPresence->getFrom()] = passedPresence;
        }
        entries_[bareJID] = jidMap;
        onPresenceChange(passedPresence);
    }
}

void PresenceOracle::handleJIDRemoved(const JID& removedJID) {
    /* 3921bis says that we don't follow up with an unavailable, so simulate this ourselves */
    Presence::ref unavailablePresence = Presence::ref(new Presence());
    unavailablePresence->setType(Presence::Unavailable);
    unavailablePresence->setFrom(removedJID);

    if (entries_.find(removedJID) != entries_.end()) {
        entries_[removedJID].clear();
        entries_[removedJID][removedJID] = unavailablePresence;
    }

    onPresenceChange(unavailablePresence);
}

Presence::ref PresenceOracle::getLastPresence(const JID& jid) const {
    PresencesMap::const_iterator i = entries_.find(jid.toBare());
    if (i == entries_.end()) {
        return Presence::ref();
    }
    PresenceMap presenceMap = i->second;
    PresenceMap::const_iterator j = presenceMap.find(jid);
    if (j != presenceMap.end()) {
        return j->second;
    }
    else {
        return Presence::ref();
    }
}

std::vector<Presence::ref> PresenceOracle::getAllPresence(const JID& bareJID) const {
    std::vector<Presence::ref> results;
    PresencesMap::const_iterator i = entries_.find(bareJID);
    if (i == entries_.end()) {
        return results;
    }
    for (const auto& jidPresence : i->second) {
        if (jidPresence.second) {
            results.push_back(jidPresence.second);
        }
    }
    return results;
}

struct PresenceAccountCmp {
    static int preferenceFromStatusShow(StatusShow::Type showType) {
        switch (showType) {
            case StatusShow::FFC:
                return 5;
            case StatusShow::Online:
                return 4;
            case StatusShow::DND:
                return 3;
            case StatusShow::Away:
                return 2;
            case StatusShow::XA:
                return 1;
            case StatusShow::None:
                return 0;
        }
        assert(false);
        return -1;
    }

    bool operator()(const Presence::ref& a, const Presence::ref& b) {
        int aPreference = preferenceFromStatusShow(a->getShow());
        int bPreference = preferenceFromStatusShow(b->getShow());

        if (aPreference != bPreference) {
            return aPreference < bPreference;
        }
        if (a->getPriority() != b->getPriority()) {
            return a->getPriority() < b->getPriority();
        }
        return a->getFrom().getResource() < b->getFrom().getResource();
    }
};

typedef std::priority_queue<Presence::ref, std::vector<Presence::ref>, PresenceAccountCmp> PresenceAccountPriorityQueue;

Presence::ref PresenceOracle::getActivePresence(const std::vector<Presence::ref> presences) {
    Presence::ref accountPresence;

    PresenceAccountPriorityQueue online;
    PresenceAccountPriorityQueue away;
    PresenceAccountPriorityQueue offline;

    for (auto&& presence : presences) {
        switch (presence->getShow()) {
            case StatusShow::Online:
                online.push(presence);
                break;
            case StatusShow::Away:
                away.push(presence);
                break;
            case StatusShow::FFC:
                online.push(presence);
                break;
            case StatusShow::XA:
                away.push(presence);
                break;
            case StatusShow::DND:
                away.push(presence);
                break;
            case StatusShow::None:
                offline.push(presence);
                break;
        }
    }

    if (!online.empty()) {
        accountPresence = online.top();
    }
    else if (!away.empty()) {
        accountPresence = away.top();
    }
    else if (!offline.empty()) {
        accountPresence = offline.top();
    }
    return accountPresence;
}

Presence::ref PresenceOracle::getAccountPresence(const JID& jid) const {
    Presence::ref accountPresence;
    std::vector<Presence::ref> allPresences = getAllPresence(jid.toBare());
    accountPresence = getActivePresence(allPresences);
    return accountPresence;
}

Presence::ref PresenceOracle::getHighestPriorityPresence(const JID& bareJID) const {
    PresencesMap::const_iterator i = entries_.find(bareJID);
    if (i == entries_.end()) {
        return Presence::ref();
    }
    Presence::ref highest;
    for (const auto& jidPresence : i->second) {
        Presence::ref current = jidPresence.second;
        if (!highest
                || current->getPriority() > highest->getPriority()
                || (current->getPriority() == highest->getPriority()
                        && StatusShow::typeToAvailabilityOrdering(current->getShow()) > StatusShow::typeToAvailabilityOrdering(highest->getShow()))) {
            highest = current;
        }
    }
    return highest;
}

}
