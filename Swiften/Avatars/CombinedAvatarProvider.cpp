/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Avatars/CombinedAvatarProvider.h>

#include <algorithm>
#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

std::string CombinedAvatarProvider::getAvatarHash(const JID& jid) const {
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
	std::string newHash = getCombinedAvatarAndCache(jid);
	if (newHash != oldHash) {
		SWIFT_LOG(debug) << "Avatar changed: " << jid << ": " << oldHash << " -> " << newHash << std::endl;
		onAvatarChanged(jid);
	}
}

std::string CombinedAvatarProvider::getCombinedAvatarAndCache(const JID& jid) const {
	SWIFT_LOG(debug) << "JID: " << jid << std::endl;
	std::string hash;
	for (size_t i = 0; i < providers.size() && hash.empty(); ++i) {
		hash = providers[i]->getAvatarHash(jid);
		SWIFT_LOG(debug) << "Provider " << providers[i] << ": " << hash << std::endl;
	}
	avatars[jid] = hash;
	return hash;
}

}
