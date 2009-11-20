#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"

#include <cassert>

#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/HMACSHA1.h"

namespace Swift {

SCRAMSHA1ClientAuthenticator::SCRAMSHA1ClientAuthenticator(const ByteArray& nonce) : ClientAuthenticator("SCRAM-SHA-1"), step(Initial), clientnonce(nonce) {
}

ByteArray SCRAMSHA1ClientAuthenticator::getResponse() const {
	if (step == Initial) {
		return getInitialClientMessage();
	}
	else {
		ByteArray mask = HMACSHA1::getResult(getClientVerifier(), initialServerMessage + getInitialClientMessage());
		ByteArray p = SHA1::getBinaryHash(getPassword());
		for (unsigned int i = 0; i < p.getSize(); ++i) {
			p[i] ^= mask[i];
		}
		return p;
	}
}

bool SCRAMSHA1ClientAuthenticator::setChallenge(const ByteArray& response) {
	if (step == Initial) {
		initialServerMessage = response;
		step = Proof;
		return getSalt().getSize() > 0;
	}
	else {
		return response == HMACSHA1::getResult(getClientVerifier(), getInitialClientMessage() + initialServerMessage);
	}
}

ByteArray SCRAMSHA1ClientAuthenticator::getSalt() const {
	if (initialServerMessage.getSize() < 8) {
		std::cerr << "ERROR: SCRAM-SHA1: Invalid server response" << std::endl;
		return ByteArray();
	}
	else {
		return ByteArray(initialServerMessage.getData(), 8);
	}
}

ByteArray SCRAMSHA1ClientAuthenticator::getClientVerifier() const {
	return HMACSHA1::getResult(SHA1::getBinaryHash(getPassword()), getSalt());
}

ByteArray SCRAMSHA1ClientAuthenticator::getInitialClientMessage() const {
	return ByteArray(getAuthorizationID()) + '\0' + ByteArray(getAuthenticationID()) + '\0' + ByteArray(clientnonce);
}

}
