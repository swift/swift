/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/SASL/DIGESTMD5ClientAuthenticator.h"

#include <cassert>

#include "Swiften/StringCodecs/MD5.h"
#include "Swiften/StringCodecs/Hexify.h"

namespace Swift {

DIGESTMD5ClientAuthenticator::DIGESTMD5ClientAuthenticator(const String& host, const String& nonce) : ClientAuthenticator("DIGEST-MD5"), step(Initial), host(host), cnonce(nonce) {
}

boost::optional<ByteArray> DIGESTMD5ClientAuthenticator::getResponse() const {
	if (step == Initial) {
		return boost::optional<ByteArray>();
	}
	else if (step == Response) {
		String realm;
		if (challenge.getValue("realm")) {
			realm = *challenge.getValue("realm");
		}
		String qop = "auth";
		String digestURI = "xmpp/" + host;
		String nc = "00000001";

		// Compute the response value
		ByteArray A1 = MD5::getHash(getAuthenticationID() + ":" + realm + ":" + getPassword()) + ":" + *challenge.getValue("nonce") + ":" + cnonce;
		if (!getAuthorizationID().isEmpty()) {
			A1 += ":" + getAuthenticationID();
		}
		String A2 = "AUTHENTICATE:" + digestURI;

		String responseValue = Hexify::hexify(MD5::getHash(
				Hexify::hexify(MD5::getHash(A1)) + ":" 
				+ *challenge.getValue("nonce") + ":" + nc + ":" + cnonce + ":" + qop + ":" 
				+ Hexify::hexify(MD5::getHash(A2))));

		DIGESTMD5Properties response;
		response.setValue("username", getAuthenticationID());
		if (!realm.isEmpty()) {
			response.setValue("realm", realm);
		}
		response.setValue("nonce", *challenge.getValue("nonce"));
		response.setValue("cnonce", cnonce);
		response.setValue("nc", "00000001");
		response.setValue("qop", qop);
		response.setValue("digest-uri", digestURI);
		response.setValue("charset", "utf-8");
		response.setValue("response", responseValue);
		if (!getAuthorizationID().isEmpty()) {
			response.setValue("authzid", getAuthorizationID());
		}
		return response.serialize();
	}
	else {
		return boost::optional<ByteArray>();
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
