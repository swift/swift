/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Avatars/CombinedAvatarProvider.h>

#include <algorithm>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

boost::optional<std::string> CombinedAvatarProvider::getAvatarHash(const JID& jid) const {
    return getCombinedAvatarAndCache(jid);
}

void CombinedAvatarProvider::addProvider(AvatarProvider* provider) {
    provider->onAvatarChanged.connect(boost::bind(&CombinedAvatarProvider::handleAvatarChanged, this, _1));
    providers.push_back(provider);
}

void CombinedAvatarProvider::removeProvider(AvatarProvider* provider) {
    std::vector<AvatarProvider*>::iterator i = std::remove(providers.begin(), providers.end(), provider);
    for(std::vector<AvatarProvider*>::iterator j = i; j < providers.end(); ++j) {
        provider->onAvatarChanged.disconnect(boost::bind(&CombinedAvatarProvider::handleAvatarChanged, this, _1));
    }
    providers.erase(i, providers.end());
}

void CombinedAvatarProvider::handleAvatarChanged(const JID& jid) {
    std::string oldHash;
    std::map<JID, std::string>::const_iterator i = avatars.find(jid);
    if (i != avatars.end()) {
        oldHash = i->second;
    }
    boost::optional<std::string> newHash = getCombinedAvatarAndCache(jid);
    if (newHash != oldHash) {
        SWIFT_LOG(debug) << "Avatar changed: " << jid << ": " << oldHash << " -> " << (newHash ? newHash.get() : "NULL") << std::endl;
        onAvatarChanged(jid);
    }
}

boost::optional<std::string> CombinedAvatarProvider::getCombinedAvatarAndCache(const JID& jid) const {
    SWIFT_LOG(debug) << "JID: " << jid << std::endl;
    boost::optional<std::string> hash;
    for (size_t i = 0; i < providers.size() && !hash; ++i) {
        hash = providers[i]->getAvatarHash(jid);
        SWIFT_LOG(debug) << "Provider " << providers[i] << ": " << (hash ? hash.get() : "NULL") << std::endl;
    }
    if (hash) {
        avatars[jid] = *hash;
    } else {
        avatars[jid] = "";
    }
    return hash;
}

}
