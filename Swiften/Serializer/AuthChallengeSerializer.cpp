/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/AuthChallengeSerializer.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

AuthChallengeSerializer::AuthChallengeSerializer() {
}

SafeByteArray AuthChallengeSerializer::serialize(boost::shared_ptr<ToplevelElement> element)  const {
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
