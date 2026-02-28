/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StanzaAckRequest.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API StanzaAckRequestSerializer : public GenericElementSerializer<StanzaAckRequest> {
        public:
            StanzaAckRequestSerializer() : GenericElementSerializer<StanzaAckRequest>() {
            }

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement>) const {
                return createSafeByteArray(XMLElement("r", "urn:xmpp:sm:2").serialize());
            }
    };
}
