/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/SASL/SCRAMSHA1ClientAuthenticator.h>

#include <cassert>
#include <map>
#include <boost/lexical_cast.hpp>

#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/StringCodecs/HMAC_SHA1.h>
#include <Swiften/StringCodecs/PBKDF2.h>
#include <Swiften/IDN/StringPrep.h>
#include <Swiften/Base/Concat.h>

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

boost::optional<SafeByteArray> SCRAMSHA1ClientAuthenticator::getResponse() const {
	if (step == Initial) {
		return createSafeByteArray(concat(getGS2Header(), getInitialBareClientMessage()));
	}
	else if (step == Proof) {
		ByteArray clientKey = HMAC_SHA1()(saltedPassword, createByteArray("Client Key"));
		ByteArray storedKey = SHA1::getHash(clientKey);
		ByteArray clientSignature = HMAC_SHA1()(createSafeByteArray(storedKey), authMessage);
		ByteArray clientProof = clientKey;
		for (unsigned int i = 0; i < clientProof.size(); ++i) {
			clientProof[i] ^= clientSignature[i];
		}
		ByteArray result = concat(getFinalMessageWithoutProof(), createByteArray(",p="), createByteArray(Base64::encode(clientProof)));
		return createSafeByteArray(result);
	}
	else {
		return boost::optional<SafeByteArray>();
	}
}

bool SCRAMSHA1ClientAuthenticator::setChallenge(const boost::optional<ByteArray>& challenge) {
	if (step == Initial) {
		if (!challenge) {
			return false;
		}
		initialServerMessage = *challenge;

		std::map<char, std::string> keys = parseMap(byteArrayToString(initialServerMessage));

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
		serverNonce = createByteArray(clientServerNonce.substr(clientnonce.size(), clientServerNonce.npos));

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
		try {
			saltedPassword = PBKDF2::encode<HMAC_SHA1>(StringPrep::getPrepared(getPassword(), StringPrep::SASLPrep), salt, iterations);
		}
		catch (const std::exception&) {
		}
		authMessage = concat(getInitialBareClientMessage(), createByteArray(","), initialServerMessage, createByteArray(","), getFinalMessageWithoutProof());
		ByteArray serverKey = HMAC_SHA1()(saltedPassword, createByteArray("Server Key"));
		serverSignature = HMAC_SHA1()(serverKey, authMessage);

		step = Proof;
		return true;
	}
	else if (step == Proof) {
		ByteArray result = concat(createByteArray("v="), createByteArray(Base64::encode(serverSignature)));
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
				result[static_cast<size_t>(key)] = value;
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
	std::string authenticationID;
	try {
		authenticationID = StringPrep::getPrepared(getAuthenticationID(), StringPrep::SASLPrep);
	}
	catch (const std::exception&) {
	}
	return createByteArray(std::string("n=" + escape(authenticationID) + ",r=" + clientnonce));
}

ByteArray SCRAMSHA1ClientAuthenticator::getGS2Header() const {
	ByteArray channelBindingHeader(createByteArray("n"));
	if (tlsChannelBindingData) {
		if (useChannelBinding) {
			channelBindingHeader = createByteArray("p=tls-unique");
		}
		else {
			channelBindingHeader = createByteArray("y");
		}
	}
	return concat(channelBindingHeader, createByteArray(","), (getAuthorizationID().empty() ? ByteArray() : createByteArray("a=" + escape(getAuthorizationID()))), createByteArray(","));
}

void SCRAMSHA1ClientAuthenticator::setTLSChannelBindingData(const ByteArray& channelBindingData) {
	this->tlsChannelBindingData = channelBindingData;
}

ByteArray SCRAMSHA1ClientAuthenticator::getFinalMessageWithoutProof() const {
	ByteArray channelBindData;
	if (useChannelBinding && tlsChannelBindingData) {
		channelBindData = *tlsChannelBindingData;
	}
	return concat(createByteArray("c=" + Base64::encode(concat(getGS2Header(), channelBindData)) + ",r=" + clientnonce), serverNonce);
}


}
