/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubSubscription.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubSubscriptionSerializer : public GenericPayloadSerializer<PubSubSubscription> {
        public:
            PubSubSubscriptionSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubSubscriptionSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubSubscription>) const override;

        private:
            static std::string serializeSubscriptionType(PubSubSubscription::SubscriptionType);

        private:
            PayloadSerializerCollection* serializers;
    };
}
