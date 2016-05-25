/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/CapsManager.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Disco/CapsStorage.h>
#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/Elements/CapsInfo.h>

namespace Swift {

CapsManager::CapsManager(CapsStorage* capsStorage, StanzaChannel* stanzaChannel, IQRouter* iqRouter, CryptoProvider* crypto) : iqRouter(iqRouter), crypto(crypto), capsStorage(capsStorage), warnOnInvalidHash(true) {
    stanzaChannel->onPresenceReceived.connect(boost::bind(&CapsManager::handlePresenceReceived, this, _1));
    stanzaChannel->onAvailableChanged.connect(boost::bind(&CapsManager::handleStanzaChannelAvailableChanged, this, _1));
}

void CapsManager::handlePresenceReceived(std::shared_ptr<Presence> presence) {
    std::shared_ptr<CapsInfo> capsInfo = presence->getPayload<CapsInfo>();
    if (!capsInfo || capsInfo->getHash() != "sha-1" || presence->getPayload<ErrorPayload>()) {
        return;
    }
    std::string hash = capsInfo->getVersion();
    if (capsStorage->getDiscoInfo(hash)) {
        return;
    }
    if (failingCaps.find(std::make_pair(presence->getFrom(), hash)) != failingCaps.end()) {
        return;
    }
    if (requestedDiscoInfos.find(hash) != requestedDiscoInfos.end()) {
        fallbacks[hash].insert(std::make_pair(presence->getFrom(), capsInfo->getNode()));
        return;
    }
    requestDiscoInfo(presence->getFrom(), capsInfo->getNode(), hash);
}

void CapsManager::handleStanzaChannelAvailableChanged(bool available) {
    if (available) {
        failingCaps.clear();
        fallbacks.clear();
        requestedDiscoInfos.clear();
    }
}

void CapsManager::handleDiscoInfoReceived(const JID& from, const std::string& hash, DiscoInfo::ref discoInfo, ErrorPayload::ref error) {
    requestedDiscoInfos.erase(hash);
    if (error || !discoInfo || CapsInfoGenerator("", crypto).generateCapsInfo(*discoInfo.get()).getVersion() != hash) {
        if (warnOnInvalidHash && !error &&  discoInfo) {
            SWIFT_LOG(warning) << "Caps from " << from.toString() << " do not verify" << std::endl;
        }
        failingCaps.insert(std::make_pair(from, hash));
        std::map<std::string, std::set< std::pair<JID, std::string> > >::iterator i = fallbacks.find(hash);
        if (i != fallbacks.end() && !i->second.empty()) {
            std::pair<JID,std::string> fallbackAndNode = *i->second.begin();
            i->second.erase(i->second.begin());
            requestDiscoInfo(fallbackAndNode.first, fallbackAndNode.second, hash);
        }
        return;
    }
    fallbacks.erase(hash);
    capsStorage->setDiscoInfo(hash, discoInfo);
    onCapsAvailable(hash);
}

void CapsManager::requestDiscoInfo(const JID& jid, const std::string& node, const std::string& hash) {
    GetDiscoInfoRequest::ref request = GetDiscoInfoRequest::create(jid, node + "#" + hash, iqRouter);
    request->onResponse.connect(boost::bind(&CapsManager::handleDiscoInfoReceived, this, jid, hash, _1, _2));
    requestedDiscoInfos.insert(hash);
    request->send();
}

DiscoInfo::ref CapsManager::getCaps(const std::string& hash) const {
    return capsStorage->getDiscoInfo(hash);
}


}
