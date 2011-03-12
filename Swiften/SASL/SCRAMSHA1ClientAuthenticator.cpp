/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"

#include <cassert>
#include <map>
#include <boost/lexical_cast.hpp>

#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/Base64.h"
#include "Swiften/StringCodecs/HMACSHA1.h"
#include "Swiften/StringCodecs/PBKDF2.h"
#include "Swiften/IDN/StringPrep.h"

namespace Swift {

static std::string escape(const std::string& s) {
	std::string result;
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] == ',') {
			result += "=2C";
		}
		else if (s[i] == '=') {
			result += "=3D";
		}
		else {
			result += s[i];
		}
	}
	return result;
}


SCRAMSHA1ClientAuthenticator::SCRAMSHA1ClientAuthenticator(const std::string& nonce, bool useChannelBinding) : ClientAuthenticator(useChannelBinding ? "SCRAM-SHA-1-PLUS" : "SCRAM-SHA-1"), step(Initial), clientnonce(nonce), useChannelBinding(useChannelBinding) {
}

boost::optional<ByteArray> SCRAMSHA1ClientAuthenticator::getResponse() const {
	if (step == Initial) {
		return getGS2Header() + getInitialBareClientMessage();
	}
	else if (step == Proof) {
		ByteArray clientKey = HMACSHA1::getResult(saltedPassword, "Client Key");
		ByteArray storedKey = SHA1::getHash(clientKey);
		ByteArray clientSignature = HMACSHA1::getResult(storedKey, authMessage);
		ByteArray clientProof = clientKey;
		for (unsigned int i = 0; i < clientProof.getSize(); ++i) {
			clientProof[i] ^= clientSignature[i];
		}
		ByteArray result = getFinalMessageWithoutProof() + ",p=" + Base64::encode(clientProof);
		return result;
	}
	else {
		return boost::optional<ByteArray>();
	}
}

bool SCRAMSHA1ClientAuthenticator::setChallenge(const boost::optional<ByteArray>& challenge) {
	if (step == Initial) {
		if (!challenge) {
			return false;
		}
		initialServerMessage = *challenge;

		std::map<char, std::string> keys = parseMap(initialServerMessage.toString());

		// Extract the salt
		ByteArray salt = Base64::decode(keys['s']);

		// Extract the server nonce
		std::string clientServerNonce = keys['r'];
		if (clientServerNonce.size() <= clientnonce.size()) {
			return false;
		}
		std::string receivedClientNonce = clientServerNonce.substr(0, clientnonce.size());
		if (receivedClientNonce != clientnonce) {
			return false;
		}
		serverNonce = clientServerNonce.substr(clientnonce.size(), clientServerNonce.npos);

		// Extract the number of iterations
		int iterations = 0;
		try {
			iterations = boost::lexical_cast<int>(keys['i']);
		}
		catch (const boost::bad_lexical_cast&) {
			return false;
		}
		if (iterations <= 0) {
			return false;
		}

		ByteArray channelBindData;
		if (useChannelBinding && tlsChannelBindingData) {
			channelBindData = *tlsChannelBindingData;
		}

		// Compute all the values needed for the server signature
		saltedPassword = PBKDF2::encode(StringPrep::getPrepared(getPassword(), StringPrep::SASLPrep), salt, iterations);
		authMessage = getInitialBareClientMessage() + "," + initialServerMessage + "," + getFinalMessageWithoutProof();
		ByteArray serverKey = HMACSHA1::getResult(saltedPassword, "Server Key");
		serverSignature = HMACSHA1::getResult(serverKey, authMessage);

		step = Proof;
		return true;
	}
	else if (step == Proof) {
		ByteArray result = ByteArray("v=") + ByteArray(Base64::encode(serverSignature));
		step = Final;
		return challenge && challenge == result;
	}
	else {
		return true;
	}
}

std::map<char, std::string> SCRAMSHA1ClientAuthenticator::parseMap(const std::string& s) {
	std::map<char, std::string> result;
	if (s.size() > 0) {
		char key = 0;
		std::string value;
		size_t i = 0;
		bool expectKey = true;
		while (i < s.size()) {
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
	std::string authenticationID = StringPrep::getPrepared(getAuthenticationID(), StringPrep::SASLPrep);
	return ByteArray(std::string("n=" + escape(authenticationID) + ",r=" + clientnonce));
}

ByteArray SCRAMSHA1ClientAuthenticator::getGS2Header() const {
	ByteArray channelBindingHeader("n");
	if (tlsChannelBindingData) {
		if (useChannelBinding) {
			channelBindingHeader = ByteArray("p=tls-unique");
		}
		else {
			channelBindingHeader = ByteArray("y");
		}
	}
	return channelBindingHeader + ByteArray(",") + (getAuthorizationID().empty() ? "" : "a=" + escape(getAuthorizationID())) + ",";
}

void SCRAMSHA1ClientAuthenticator::setTLSChannelBindingData(const ByteArray& channelBindingData) {
	this->tlsChannelBindingData = channelBindingData;
}

ByteArray SCRAMSHA1ClientAuthenticator::getFinalMessageWithoutProof() const {
	ByteArray channelBindData;
	if (useChannelBinding && tlsChannelBindingData) {
		channelBindData = *tlsChannelBindingData;
	}
	return ByteArray("c=") + Base64::encode(getGS2Header() + channelBindData) + ",r=" + clientnonce + serverNonce;
}


}
