/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API ChatStateSerializer : public GenericPayloadSerializer<ChatState> {
        public:
            ChatStateSerializer();

            virtual std::string serializePayload(std::shared_ptr<ChatState> error)  const;
    };
}
