/*
 * Copyright (c) 2011-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MUCDestroyPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API MUCDestroyPayloadSerializer : public GenericPayloadSerializer<MUCDestroyPayload> {
        public:
            MUCDestroyPayloadSerializer();
            virtual std::string serializePayload(std::shared_ptr<MUCDestroyPayload> version)  const;
    };
}

