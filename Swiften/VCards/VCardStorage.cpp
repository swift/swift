/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/VCards/VCardStorage.h>

#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/Crypto/CryptoProvider.h>

namespace Swift {

VCardStorage::VCardStorage(CryptoProvider* crypto) : crypto(crypto) {
}

VCardStorage::~VCardStorage() {
}

std::string VCardStorage::getPhotoHash(const JID& jid) const {
	VCard::ref vCard = getVCard(jid);
	if (vCard && !vCard->getPhoto().empty()) {
		return Hexify::hexify(crypto->getSHA1Hash(vCard->getPhoto()));
	}
	else {
		return "";
	}
}

}
