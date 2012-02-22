/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/SASL/DIGESTMD5ClientAuthenticator.h>

#include <cassert>

#include <Swiften/StringCodecs/MD5.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Algorithm.h>

namespace Swift {

DIGESTMD5ClientAuthenticator::DIGESTMD5ClientAuthenticator(const std::string& host, const std::string& nonce) : ClientAuthenticator("DIGEST-MD5"), step(Initial), host(host), cnonce(nonce) {
}

bool DIGESTMD5ClientAuthenticator::canBeUsed() {
	return MD5::isAllowedForCrypto();
}

boost::optional<SafeByteArray> DIGESTMD5ClientAuthenticator::getResponse() const {
	if (step == Initial) {
		return boost::optional<SafeByteArray>();
	}
	else if (step == Response) {
		std::string realm;
		if (challenge.getValue("realm")) {
			realm = *challenge.getValue("realm");
		}
		std::string qop = "auth";
		std::string digestURI = "xmpp/" + host;
		std::string nc = "00000001";

		// Compute the response value
		ByteArray A1 = concat(
				MD5::getHash(
					concat(createSafeByteArray(getAuthenticationID().c_str()), createSafeByteArray(":"), createSafeByteArray(realm.c_str()), createSafeByteArray(":"), getPassword())),
				createByteArray(":"), createByteArray(*challenge.getValue("nonce")), createByteArray(":"), createByteArray(cnonce));
		if (!getAuthorizationID().empty()) {
			append(A1, createByteArray(":" + getAuthenticationID()));
		}
		ByteArray A2 = createByteArray("AUTHENTICATE:" + digestURI);

		std::string responseValue = Hexify::hexify(MD5::getHash(createByteArray(
			Hexify::hexify(MD5::getHash(A1)) + ":"
			+ *challenge.getValue("nonce") + ":" + nc + ":" + cnonce + ":" + qop + ":"
			+ Hexify::hexify(MD5::getHash(A2)))));


		DIGESTMD5Properties response;
		response.setValue("username", getAuthenticationID());
		if (!realm.empty()) {
			response.setValue("realm", realm);
		}
		response.setValue("nonce", *challenge.getValue("nonce"));
		response.setValue("cnonce", cnonce);
		response.setValue("nc", "00000001");
		response.setValue("qop", qop);
		response.setValue("digest-uri", digestURI);
		response.setValue("charset", "utf-8");
		response.setValue("response", responseValue);
		if (!getAuthorizationID().empty()) {
			response.setValue("authzid", getAuthorizationID());
		}
		return createSafeByteArray(response.serialize());
	}
	else {
		return boost::optional<SafeByteArray>();
	}
}

bool DIGESTMD5ClientAuthenticator::setChallenge(const boost::optional<ByteArray>& challengeData) {
	if (step == Initial) {
		if (!challengeData) {
			return false;
		}
		challenge = DIGESTMD5Properties::parse(*challengeData);

		// Sanity checks
		if (!challenge.getValue("nonce")) {
			return false;
		}
		if (!challenge.getValue("charset") || *challenge.getValue("charset") != "utf-8") {
			return false;
		}
		step = Response;
		return true;
	}
	else {
		step = Final;
		// TODO: Check RSPAuth
		return true;
	}
}

}
