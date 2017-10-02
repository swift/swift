/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerPubSub.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerPubSubSerializer : public GenericPayloadSerializer<PubSubOwnerPubSub> {
        public:
            PubSubOwnerPubSubSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerPubSubSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerPubSub>) const override;

        private:
            std::vector< std::shared_ptr<PayloadSerializer> > pubsubSerializers;
    };
}
