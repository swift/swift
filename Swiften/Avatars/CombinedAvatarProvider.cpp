/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Avatars/CombinedAvatarProvider.h"

#include <algorithm>
#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

std::string CombinedAvatarProvider::getAvatarHash(const JID& jid) const {
	SWIFT_LOG(debug) << "JID: " << jid << std::endl;
	for (size_t i = 0; i < providers.size(); ++i) {
		std::string hash = providers[i]->getAvatarHash(jid);
		SWIFT_LOG(debug) << "Provider " << providers[i] << ": " << hash << std::endl;
		if (!hash.empty()) {
			return hash;
		}
	}
	SWIFT_LOG(debug) << "No hash found" << std::endl;
	return std::string();
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
	std::string hash = getAvatarHash(jid);
	std::map<JID, std::string>::iterator i = avatars.find(jid);
	if (i != avatars.end()) {
		if (i->second != hash) {
			if (hash.empty()) {
				avatars.erase(i);
			}
			else {
				avatars.insert(std::make_pair(jid, hash));
			}
			onAvatarChanged(jid);
		}
	}
	else if (!hash.empty()) {
		avatars.insert(std::make_pair(jid, hash));
		onAvatarChanged(jid);
	}
}

}
