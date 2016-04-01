/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/PayloadSerializer.h>

namespace Swift {
    template<typename PAYLOAD_TYPE>
    class GenericPayloadSerializer : public PayloadSerializer {
        public:
            virtual std::string serialize(std::shared_ptr<Payload> element)  const {
                return serializePayload(std::dynamic_pointer_cast<PAYLOAD_TYPE>(element));
            }

            virtual bool canSerialize(std::shared_ptr<Payload> element) const {
                return !!std::dynamic_pointer_cast<PAYLOAD_TYPE>(element);
            }

            virtual std::string serializePayload(std::shared_ptr<PAYLOAD_TYPE>) const = 0;
    };
}
