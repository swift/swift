/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/AuthChallengeSerializer.h"

#include "Swiften/Elements/AuthChallenge.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

AuthChallengeSerializer::AuthChallengeSerializer() {
}

std::string AuthChallengeSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<AuthChallenge> authChallenge(boost::dynamic_pointer_cast<AuthChallenge>(element));
	std::string value;
	boost::optional<ByteArray> message = authChallenge->getValue();
	if (message) {
		if ((*message).isEmpty()) {
			value = "=";
		}
		else {
			value = Base64::encode(*message);
		}
	}
	return "<challenge xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">" + value + "</challenge>";
}

}
