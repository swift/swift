/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventItems.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventItemsSerializer : public GenericPayloadSerializer<PubSubEventItems> {
        public:
            PubSubEventItemsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventItemsSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventItems>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
