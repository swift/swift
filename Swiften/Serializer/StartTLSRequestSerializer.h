/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StartTLSRequest.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API StartTLSRequestSerializer : public GenericElementSerializer<StartTLSRequest> {
        public:
            StartTLSRequestSerializer() : GenericElementSerializer<StartTLSRequest>() {
            }

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement>) const {
                return createSafeByteArray(XMLElement("starttls", "urn:ietf:params:xml:ns:xmpp-tls").serialize());
            }
    };
}
