/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/SASL/PLAINClientAuthenticator.h>
#include <Swiften/Base/Concat.h>

namespace Swift {

PLAINClientAuthenticator::PLAINClientAuthenticator() : ClientAuthenticator("PLAIN") {
}

boost::optional<SafeByteArray> PLAINClientAuthenticator::getResponse() const {
	return concat(createSafeByteArray(getAuthorizationID()), createSafeByteArray('\0'), createSafeByteArray(getAuthenticationID()), createSafeByteArray('\0'), getPassword());
}

bool PLAINClientAuthenticator::setChallenge(const boost::optional<ByteArray>&) {
	return true;
}

}
