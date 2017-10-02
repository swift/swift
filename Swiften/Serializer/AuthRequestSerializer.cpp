/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/AuthRequestSerializer.h>

#include <Swiften/Base/Concat.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

AuthRequestSerializer::AuthRequestSerializer() {
}

SafeByteArray AuthRequestSerializer::serialize(std::shared_ptr<ToplevelElement> element)  const {
    std::shared_ptr<AuthRequest> authRequest(std::dynamic_pointer_cast<AuthRequest>(element));
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
