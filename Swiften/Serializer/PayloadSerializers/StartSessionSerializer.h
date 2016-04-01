/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StartSession.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API StartSessionSerializer : public GenericPayloadSerializer<StartSession> {
        public:
            StartSessionSerializer() : GenericPayloadSerializer<StartSession>() {}

            virtual std::string serializePayload(std::shared_ptr<StartSession>)  const {
                return XMLElement("session", "urn:ietf:params:xml:ns:xmpp-session").serialize();
            }
    };
}
