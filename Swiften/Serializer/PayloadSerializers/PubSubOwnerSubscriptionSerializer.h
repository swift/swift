/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerSubscription.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerSubscriptionSerializer : public GenericPayloadSerializer<PubSubOwnerSubscription> {
        public:
            PubSubOwnerSubscriptionSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerSubscriptionSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerSubscription>) const override;

        private:
            static std::string serializeSubscriptionType(PubSubOwnerSubscription::SubscriptionType);
    };
}
