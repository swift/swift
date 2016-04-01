/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerSubscriptions.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerSubscriptionsSerializer : public GenericPayloadSerializer<PubSubOwnerSubscriptions> {
        public:
            PubSubOwnerSubscriptionsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerSubscriptionsSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerSubscriptions>) const SWIFTEN_OVERRIDE;

        private:


        private:
            PayloadSerializerCollection* serializers;
    };
}
