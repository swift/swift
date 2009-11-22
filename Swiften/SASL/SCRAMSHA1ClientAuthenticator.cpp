#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"

#include <cassert>
#include <map>
#include <boost/lexical_cast.hpp>

#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/Base64.h"
#include "Swiften/StringCodecs/HMACSHA1.h"
#include "Swiften/StringCodecs/PBKDF2.h"

namespace Swift {

SCRAMSHA1ClientAuthenticator::SCRAMSHA1ClientAuthenticator(const String& nonce) : ClientAuthenticator("SCRAM-SHA-1"), step(Initial), clientnonce(nonce) {
	// TODO: Normalize authentication id
	// TODO: Normalize getPassword()
}

ByteArray SCRAMSHA1ClientAuthenticator::getResponse() {
	if (step == Initial) {
		return "n,," + getInitialBareClientMessage();
	}
	else {
		ByteArray saltedPassword = PBKDF2::encode(getPassword(), salt, iterations);
		ByteArray clientKey = HMACSHA1::getResult(saltedPassword, "Client Key");
		ByteArray storedKey = SHA1::getBinaryHash(clientKey);
		ByteArray serverKey = HMACSHA1::getResult(saltedPassword, "Server Key");

		ByteArray authMessage = getInitialBareClientMessage() + "," + initialServerMessage + "," + "c=biwsCg==," + "r=" + clientnonce + serverNonce;
		ByteArray clientSignature = HMACSHA1::getResult(storedKey, authMessage);
		serverSignature = HMACSHA1::getResult(serverKey, authMessage);
		ByteArray clientProof = clientKey;
		for (unsigned int i = 0; i < clientProof.getSize(); ++i) {
			clientProof[i] ^= clientSignature[i];
		}
		ByteArray result = ByteArray("c=biwsCg==,r=") + clientnonce + serverNonce + ",p=" + Base64::encode(clientProof);
		return result;
	}
}

bool SCRAMSHA1ClientAuthenticator::setChallenge(const ByteArray& challenge) {
	if (step == Initial) {
		initialServerMessage = challenge;

		// TODO: Check if these values are correct
		std::map<char, String> keys = parseMap(String(initialServerMessage.getData(), initialServerMessage.getSize()));
		salt = Base64::decode(keys['s']);
		String clientServerNonce = keys['r'];
		serverNonce = clientServerNonce.getSubstring(clientnonce.getUTF8Size(), clientServerNonce.npos());
		iterations = boost::lexical_cast<int>(keys['i'].getUTF8String());

		step = Proof;
		return true;
	}
	else {
		return challenge == Base64::encode(ByteArray("v=") + Base64::encode(serverSignature));
	}
}

std::map<char, String> SCRAMSHA1ClientAuthenticator::parseMap(const String& s) {
	// TODO: Do some proper checking here
	std::map<char, String> result;
	if (s.getUTF8Size() > 0) {
		char key;
		String value;
		size_t i = 0;
		bool expectKey = true;
		while (i < s.getUTF8Size()) {
			if (expectKey) {
				key = s[i];
				expectKey = false;
				i++;
			}
			else if (s[i] == ',') {
				result[key] = value;
				value = "";
				expectKey = true;
			}
			else {
				value += s[i];
			}
			i++;
		}
		result[key] = value;
	}
	return result;
}

ByteArray SCRAMSHA1ClientAuthenticator::getInitialBareClientMessage() const {
	String authenticationID = getAuthenticationID();
	String escapedAuthenticationID;
	for (size_t i = 0; i < authenticationID.getUTF8Size(); ++i) {
		if (authenticationID[i] == ',') {
			escapedAuthenticationID += "=2C";
		}
		else if (authenticationID[i] == '=') {
			escapedAuthenticationID += "=3D";
		}
		else {
			escapedAuthenticationID += authenticationID[i];
		}
	}
	return ByteArray(String("n=" + escapedAuthenticationID + ",r=" + clientnonce));
}

}
