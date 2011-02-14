/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/AuthSuccessSerializer.h"

#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

AuthSuccessSerializer::AuthSuccessSerializer() {
}

std::string AuthSuccessSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<AuthSuccess> authSuccess(boost::dynamic_pointer_cast<AuthSuccess>(element));
	std::string value;
	boost::optional<ByteArray> message = authSuccess->getValue();
	if (message) {
		if ((*message).isEmpty()) {
			value = "=";
		}
		else {
			value = Base64::encode(*message);
		}
	}
	return "<success xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">" + value + "</success>";
}

}
