#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"

#include <cassert>
#include <map>
#include <boost/lexical_cast.hpp>

#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/Base64.h"
#include "Swiften/StringCodecs/HMACSHA1.h"
#include "Swiften/StringCodecs/PBKDF2.h"
#include "Swiften/StringPrep/StringPrep.h"

namespace Swift {

SCRAMSHA1ClientAuthenticator::SCRAMSHA1ClientAuthenticator(const String& nonce) : ClientAuthenticator("SCRAM-SHA-1"), step(Initial), clientnonce(nonce) {
}

ByteArray SCRAMSHA1ClientAuthenticator::getResponse() const {
	if (step == Initial) {
		return "n,," + getInitialBareClientMessage();
	}
	else {
		ByteArray clientKey = HMACSHA1::getResult(saltedPassword, "Client Key");
		ByteArray storedKey = SHA1::getBinaryHash(clientKey);
		ByteArray clientSignature = HMACSHA1::getResult(storedKey, authMessage);
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

		std::map<char, String> keys = parseMap(String(initialServerMessage.getData(), initialServerMessage.getSize()));

		// Extract the salt
		ByteArray salt = Base64::decode(keys['s']);

		// Extract the server nonce
		String clientServerNonce = keys['r'];
		if (clientServerNonce.getUTF8Size() <= clientnonce.getUTF8Size()) {
			return false;
		}
		String receivedClientNonce = clientServerNonce.getSubstring(0, clientnonce.getUTF8Size());
		if (receivedClientNonce != clientnonce) {
			return false;
		}
		serverNonce = clientServerNonce.getSubstring(clientnonce.getUTF8Size(), clientServerNonce.npos());

		// Extract the number of iterations
		int iterations = 0;
		try {
			iterations = boost::lexical_cast<int>(keys['i'].getUTF8String());
		}
		catch (const boost::bad_lexical_cast&) {
			return false;
		}
		if (iterations <= 0) {
			return false;
		}

		// Compute all the values needed for the server signature
		saltedPassword = PBKDF2::encode(StringPrep::getPrepared(getPassword(), StringPrep::SASLPrep), salt, iterations);
		authMessage = getInitialBareClientMessage() + "," + initialServerMessage + "," + "c=biwsCg==," + "r=" + clientnonce + serverNonce;
		ByteArray serverKey = HMACSHA1::getResult(saltedPassword, "Server Key");
		serverSignature = HMACSHA1::getResult(serverKey, authMessage);

		step = Proof;
		return true;
	}
	else {
		ByteArray result = ByteArray("v=") + ByteArray(Base64::encode(serverSignature));
		return challenge == result;
	}
}

std::map<char, String> SCRAMSHA1ClientAuthenticator::parseMap(const String& s) {
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
	String authenticationID = StringPrep::getPrepared(getAuthenticationID(), StringPrep::SASLPrep);
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
