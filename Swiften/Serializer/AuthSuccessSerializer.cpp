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

String AuthSuccessSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<AuthSuccess> authRequest(boost::dynamic_pointer_cast<AuthSuccess>(element));
	String value = (authRequest->getValue().isEmpty() ? "=" : Base64::encode(authRequest->getValue()));
	return "<success xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">" + value + "</success>";
}

}
