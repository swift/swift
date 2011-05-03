/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/AuthResponseSerializer.h>

#include <Swiften/Elements/AuthResponse.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {

AuthResponseSerializer::AuthResponseSerializer() {
}

std::string AuthResponseSerializer::serialize(boost::shared_ptr<Element> element)	const {
	boost::shared_ptr<AuthResponse> authResponse(boost::dynamic_pointer_cast<AuthResponse>(element));
	std::string value;
	boost::optional<std::vector<unsigned char> > message = authResponse->getValue();
	if (message) {
		if ((*message).empty()) {
			value = "=";
		}
		else {
			value = Base64::encode(ByteArray(*message));
		}
	}
	return "<response xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">" + value + "</response>";
}

}
