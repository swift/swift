/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/AuthRequestSerializer.h>

#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/Concat.h>

namespace Swift {

AuthRequestSerializer::AuthRequestSerializer() {
}

SafeByteArray AuthRequestSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<AuthRequest> authRequest(boost::dynamic_pointer_cast<AuthRequest>(element));
	SafeByteArray value;
	boost::optional<SafeByteArray> message = authRequest->getMessage();
	if (message) {
		if ((*message).empty()) {
			value = createSafeByteArray("=");
		}
		else {
			value = Base64::encode(*message);
		}
	}
	return concat(createSafeByteArray("<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"" + authRequest->getMechanism() + "\">"), value, createSafeByteArray("</auth>"));
}

}
