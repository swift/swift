/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/SASL/EXTERNALClientAuthenticator.h>

namespace Swift {

EXTERNALClientAuthenticator::EXTERNALClientAuthenticator() : ClientAuthenticator("EXTERNAL"), finished(false) {
}

boost::optional<SafeByteArray> EXTERNALClientAuthenticator::getResponse() const {
	return boost::optional<SafeByteArray>();
}

bool EXTERNALClientAuthenticator::setChallenge(const boost::optional<ByteArray>&) {
	if (finished) {
		return false;
	}
	finished = true;
	return true;
}

}
