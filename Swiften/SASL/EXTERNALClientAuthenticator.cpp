/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/SASL/EXTERNALClientAuthenticator.h>

namespace Swift {

EXTERNALClientAuthenticator::EXTERNALClientAuthenticator() : ClientAuthenticator("EXTERNAL"), finished(false) {
}

boost::optional<SafeByteArray> EXTERNALClientAuthenticator::getResponse() const {
    const std::string& authorizationID = getAuthorizationID();

    if (authorizationID.empty()) {
        return boost::optional<SafeByteArray>();
    } else {
        return createSafeByteArray(authorizationID);
    }
}

bool EXTERNALClientAuthenticator::setChallenge(const boost::optional<ByteArray>&) {
    if (finished) {
        return false;
    }
    finished = true;
    return true;
}

}
