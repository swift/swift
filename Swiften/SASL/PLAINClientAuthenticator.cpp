#include "Swiften/SASL/PLAINClientAuthenticator.h"

namespace Swift {

PLAINClientAuthenticator::PLAINClientAuthenticator() : ClientAuthenticator("PLAIN") {
}

ByteArray PLAINClientAuthenticator::getResponse() const {
	return ByteArray(getAuthorizationID()) + '\0' + ByteArray(getAuthenticationID()) + '\0' + ByteArray(getPassword());
}

bool PLAINClientAuthenticator::setChallenge(const ByteArray&) {
	return true;
}

}
