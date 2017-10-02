/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventSubscription.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventSubscriptionSerializer : public GenericPayloadSerializer<PubSubEventSubscription> {
        public:
            PubSubEventSubscriptionSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventSubscriptionSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventSubscription>) const override;

        private:
            static std::string serializeSubscriptionType(PubSubEventSubscription::SubscriptionType);
    };
}
