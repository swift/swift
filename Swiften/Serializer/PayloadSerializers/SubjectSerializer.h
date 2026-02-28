/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Subject.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {
    class SWIFTEN_API SubjectSerializer : public GenericPayloadSerializer<Subject> {
        public:
            SubjectSerializer() : GenericPayloadSerializer<Subject>() {}

            virtual std::string serializePayload(std::shared_ptr<Subject> subject)  const {
                XMLTextNode textNode(subject->getText());
                return "<subject>" + textNode.serialize() + "</subject>";
            }
    };
}
