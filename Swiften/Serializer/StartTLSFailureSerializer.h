/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StartTLSFailure.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API StartTLSFailureSerializer : public GenericElementSerializer<StartTLSFailure> {
        public:
            StartTLSFailureSerializer() : GenericElementSerializer<StartTLSFailure>() {
            }

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement>) const {
                return createSafeByteArray(XMLElement("failure", "urn:ietf:params:xml:ns:xmpp-tls").serialize());
            }
    };
}
