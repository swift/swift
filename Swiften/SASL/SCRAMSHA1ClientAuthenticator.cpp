#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"

#include <cassert>

#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/HMACSHA1.h"

namespace Swift {

SCRAMSHA1ClientAuthenticator::SCRAMSHA1ClientAuthenticator(const String& authcid, const String& password, const String& authzid, const ByteArray& nonce) : step(Initial), authcid(authcid), password(password), authzid(authzid), clientnonce(nonce) {
}

ByteArray SCRAMSHA1ClientAuthenticator::getMessage() const {
	if (step == Initial) {
		return getInitialClientMessage();
	}
	else {
		ByteArray mask = HMACSHA1::getResult(getClientVerifier(), initialServerMessage + getInitialClientMessage());
		ByteArray p = SHA1::getBinaryHash(password);
		for (unsigned int i = 0; i < p.getSize(); ++i) {
			p[i] ^= mask[i];
		}
		return p;
	}
}

bool SCRAMSHA1ClientAuthenticator::setResponse(const ByteArray& response) {
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
	return HMACSHA1::getResult(SHA1::getBinaryHash(password), getSalt());
}

ByteArray SCRAMSHA1ClientAuthenticator::getInitialClientMessage() const {
	return ByteArray(authzid) + '\0' + ByteArray(authcid) + '\0' + ByteArray(clientnonce);
}

}
