/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/VCards/VCardStorage.h>

#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/StringCodecs/SHA1.h>

namespace Swift {

VCardStorage::~VCardStorage() {
}

std::string VCardStorage::getPhotoHash(const JID& jid) const {
	VCard::ref vCard = getVCard(jid);
	if (vCard && !vCard->getPhoto().empty()) {
		return Hexify::hexify(SHA1::getHash(vCard->getPhoto()));
	}
	else {
		return "";
	}
}

}
