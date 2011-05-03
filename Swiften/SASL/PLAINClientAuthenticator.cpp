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

boost::optional<ByteArray> PLAINClientAuthenticator::getResponse() const {
	return concat(createByteArray(getAuthorizationID()), createByteArray('\0'), createByteArray(getAuthenticationID()), createByteArray('\0'), createByteArray(getPassword()));
}

bool PLAINClientAuthenticator::setChallenge(const boost::optional<ByteArray>&) {
	return true;
}

}
