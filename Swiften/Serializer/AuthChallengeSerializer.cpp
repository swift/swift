/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/AuthChallengeSerializer.h>

#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {

AuthChallengeSerializer::AuthChallengeSerializer() {
}

SafeByteArray AuthChallengeSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<AuthChallenge> authChallenge(boost::dynamic_pointer_cast<AuthChallenge>(element));
	std::string value;
	boost::optional<std::vector<unsigned char> > message = authChallenge->getValue();
	if (message) {
		if ((*message).empty()) {
			value = "=";
		}
		else {
			value = Base64::encode(ByteArray(*message));
		}
	}
	return createSafeByteArray("<challenge xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">" + value + "</challenge>");
}

}
