/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/SASL/PLAINClientAuthenticator.h"

namespace Swift {

PLAINClientAuthenticator::PLAINClientAuthenticator() : ClientAuthenticator("PLAIN") {
}

boost::optional<ByteArray> PLAINClientAuthenticator::getResponse() const {
	return ByteArray(getAuthorizationID()) + '\0' + ByteArray(getAuthenticationID()) + '\0' + ByteArray(getPassword());
}

bool PLAINClientAuthenticator::setChallenge(const boost::optional<ByteArray>&) {
	return true;
}

}
