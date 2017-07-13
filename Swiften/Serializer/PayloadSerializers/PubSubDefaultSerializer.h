/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubDefault.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubDefaultSerializer : public GenericPayloadSerializer<PubSubDefault> {
        public:
            PubSubDefaultSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubDefaultSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubDefault>) const override;

        private:
            static std::string serializeType(PubSubDefault::Type);
    };
}
