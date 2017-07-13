/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventItem.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventItemSerializer : public GenericPayloadSerializer<PubSubEventItem> {
        public:
            PubSubEventItemSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventItemSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventItem>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
