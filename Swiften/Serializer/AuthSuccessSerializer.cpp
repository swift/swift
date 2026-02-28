/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/AuthSuccessSerializer.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/AuthSuccess.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

AuthSuccessSerializer::AuthSuccessSerializer() {
}

SafeByteArray AuthSuccessSerializer::serialize(std::shared_ptr<ToplevelElement> element)  const {
    std::shared_ptr<AuthSuccess> authSuccess(std::dynamic_pointer_cast<AuthSuccess>(element));
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
