/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/AuthSuccessSerializer.h>

#include <Swiften/Elements/AuthSuccess.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {

AuthSuccessSerializer::AuthSuccessSerializer() {
}

SafeByteArray AuthSuccessSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<AuthSuccess> authSuccess(boost::dynamic_pointer_cast<AuthSuccess>(element));
	std::string value;
	boost::optional<std::vector<unsigned char> > message = authSuccess->getValue();
	if (message) {
		if ((*message).empty()) {
			value = "=";
		}
		else {
			value = Base64::encode(ByteArray(*message));
		}
	}
	return createSafeByteArray("<success xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">" + value + "</success>");
}

}
