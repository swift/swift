/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/Certificate.h>

#include <sstream>

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/StringCodecs/Hexify.h>

namespace Swift {

const char* Certificate::ID_ON_XMPPADDR_OID = "1.3.6.1.5.5.7.8.5";
const char* Certificate::ID_ON_DNSSRV_OID = "1.3.6.1.5.5.7.8.7";

Certificate::~Certificate() {
}

std::string Certificate::getSHA1Fingerprint(Certificate::ref certificate, CryptoProvider* crypto) {
    ByteArray hash = crypto->getSHA1Hash(certificate->toDER());
    std::ostringstream s;
    for (size_t i = 0; i < hash.size(); ++i) {
        if (i > 0) {
            s << ":";
        }
        s << Hexify::hexify(hash[i]);
    }
    return std::string(s.str());
}

}
