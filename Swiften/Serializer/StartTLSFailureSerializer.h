/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StartTLSFailure.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API StartTLSFailureSerializer : public GenericElementSerializer<StartTLSFailure> {
        public:
            StartTLSFailureSerializer() : GenericElementSerializer<StartTLSFailure>() {
            }

            virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const {
                return createSafeByteArray(XMLElement("failure", "urn:ietf:params:xml:ns:xmpp-tls").serialize());
            }
    };
}
