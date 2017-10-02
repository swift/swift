/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/EntityCapsManager.h>

#include <boost/bind.hpp>

#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Disco/CapsProvider.h>

namespace Swift {

EntityCapsManager::EntityCapsManager(CapsProvider* capsProvider, StanzaChannel* stanzaChannel) : capsProvider(capsProvider) {
    stanzaChannel->onPresenceReceived.connect(boost::bind(&EntityCapsManager::handlePresenceReceived, this, _1));
    stanzaChannel->onAvailableChanged.connect(boost::bind(&EntityCapsManager::handleStanzaChannelAvailableChanged, this, _1));
    capsProvider->onCapsAvailable.connect(boost::bind(&EntityCapsManager::handleCapsAvailable, this, _1));
}

void EntityCapsManager::handlePresenceReceived(std::shared_ptr<Presence> presence) {
    JID from = presence->getFrom();
    if (presence->isAvailable()) {
        std::shared_ptr<CapsInfo> capsInfo = presence->getPayload<CapsInfo>();
        if (!capsInfo || capsInfo->getHash() != "sha-1" || presence->getPayload<ErrorPayload>()) {
            return;
        }
        std::string hash = capsInfo->getVersion();
        std::map<JID, std::string>::iterator i = caps.find(from);
        if (i == caps.end() || i->second != hash) {
            caps.insert(std::make_pair(from, hash));
            DiscoInfo::ref disco = capsProvider->getCaps(hash);
            if (disco) {
                onCapsChanged(from);
            }
            else if (i != caps.end()) {
                caps.erase(i);
                onCapsChanged(from);
            }
        }
    }
    else {
        std::map<JID, std::string>::iterator i = caps.find(from);
        if (i != caps.end()) {
            caps.erase(i);
            onCapsChanged(from);
        }
    }
}

void EntityCapsManager::handleStanzaChannelAvailableChanged(bool available) {
    if (available) {
        std::map<JID, std::string> capsCopy;
        capsCopy.swap(caps);
        for (std::map<JID,std::string>::const_iterator i = capsCopy.begin(); i != capsCopy.end(); ++i) {
            onCapsChanged(i->first);
        }
    }
}

void EntityCapsManager::handleCapsAvailable(const std::string& hash) {
    // TODO: Use Boost.Bimap ?
    for (std::map<JID,std::string>::const_iterator i = caps.begin(); i != caps.end(); ++i) {
        if (i->second == hash) {
            onCapsChanged(i->first);
        }
    }
}

DiscoInfo::ref EntityCapsManager::getCaps(const JID& jid) const {
    std::map<JID, std::string>::const_iterator i = caps.find(jid);
    if (i != caps.end()) {
        return capsProvider->getCaps(i->second);
    }
    return DiscoInfo::ref();
}

DiscoInfo::ref EntityCapsManager::getCapsCached(const JID& jid) {
    DiscoInfo::ref result;
    auto capsHit = caps.find(jid);
    if (capsHit != caps.end()) {
        result = lruDiscoCache.get(capsHit->second, [&](const std::string& capsHash) {
            boost::optional<DiscoInfo::ref> fileCacheResult;
            auto fileCacheDiscoInfo = capsProvider->getCaps(capsHash);
            if (fileCacheDiscoInfo) {
                fileCacheResult = fileCacheDiscoInfo;
            }
            return fileCacheResult;
        }).get_value_or(DiscoInfo::ref());
    }
    return result;
}

}
