/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/TLSProceed.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API TLSProceedSerializer : public GenericElementSerializer<TLSProceed> {
        public:
            TLSProceedSerializer() : GenericElementSerializer<TLSProceed>() {
            }

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement>) const {
                return createSafeByteArray(XMLElement("proceed", "urn:ietf:params:xml:ns:xmpp-tls").serialize());
            }
    };
}
