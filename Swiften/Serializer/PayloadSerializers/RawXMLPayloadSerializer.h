/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API RawXMLPayloadSerializer : public GenericPayloadSerializer<RawXMLPayload> {
        public:
            RawXMLPayloadSerializer() : GenericPayloadSerializer<RawXMLPayload>() {}

            virtual std::string serializePayload(std::shared_ptr<RawXMLPayload> p)  const {
                return p->getRawXML();
            }
    };
}
