/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/AuthResponseSerializer.h"

#include "Swiften/Elements/AuthResponse.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

AuthResponseSerializer::AuthResponseSerializer() {
}

String AuthResponseSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<AuthResponse> authRequest(boost::dynamic_pointer_cast<AuthResponse>(element));
	String value = (authRequest->getValue().isEmpty() ? "=" : Base64::encode(authRequest->getValue()));
	return "<response xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">" + value + "</response>";
}

}
