/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/AuthResponseSerializer.h>

#include <Swiften/Elements/AuthResponse.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/Concat.h>

namespace Swift {

AuthResponseSerializer::AuthResponseSerializer() {
}

SafeByteArray AuthResponseSerializer::serialize(boost::shared_ptr<ToplevelElement> element)	const {
	boost::shared_ptr<AuthResponse> authResponse(boost::dynamic_pointer_cast<AuthResponse>(element));
	SafeByteArray value;
	boost::optional<SafeByteArray> message = authResponse->getValue();
	if (message) {
		if ((*message).empty()) {
			value = createSafeByteArray("");
		}
		else {
			value = Base64::encode(*message);
		}
	}
	return concat(createSafeByteArray("<response xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"), value, createSafeByteArray("</response>"));
}

}
