/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubItemsSerializer : public GenericPayloadSerializer<PubSubItems> {
        public:
            PubSubItemsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubItemsSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubItems>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
