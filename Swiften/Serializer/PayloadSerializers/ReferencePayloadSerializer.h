/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ReferencePayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API ReferencePayloadSerializer : public GenericPayloadSerializer<ReferencePayload> {
        public:
            ReferencePayloadSerializer(PayloadSerializerCollection* payloadSerializers);
            virtual std::string serializePayload(ReferencePayload::ref reference)  const;

        private:
            const std::string getTypeString(const ReferencePayload::Type type) const;

            PayloadSerializerCollection* payloadSerializers_;
    };
}
