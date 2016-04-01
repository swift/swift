/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Body.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {
    class SWIFTEN_API BodySerializer : public GenericPayloadSerializer<Body> {
        public:
            BodySerializer() : GenericPayloadSerializer<Body>() {}

            virtual std::string serializePayload(std::shared_ptr<Body> body)  const {
                XMLTextNode textNode(body->getText());
                return "<body>" + textNode.serialize() + "</body>";
            }
    };
}
