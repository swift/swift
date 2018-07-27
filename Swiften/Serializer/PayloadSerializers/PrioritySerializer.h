/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Priority.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API PrioritySerializer : public GenericPayloadSerializer<Priority> {
        public:
            PrioritySerializer() : GenericPayloadSerializer<Priority>() {}

            virtual std::string serializePayload(std::shared_ptr<Priority> priority)  const {
                return "<priority>" + std::to_string(priority->getPriority()) + "</priority>";
            }
    };
}
