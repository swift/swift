/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/VCards/VCardStorage.h>

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/StringCodecs/Hexify.h>

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
