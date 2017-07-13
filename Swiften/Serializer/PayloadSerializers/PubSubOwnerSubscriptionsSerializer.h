/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerSubscriptions.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerSubscriptionsSerializer : public GenericPayloadSerializer<PubSubOwnerSubscriptions> {
        public:
            PubSubOwnerSubscriptionsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerSubscriptionsSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerSubscriptions>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
