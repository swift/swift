/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubSubscriptions.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubSubscriptionsSerializer : public GenericPayloadSerializer<PubSubSubscriptions> {
        public:
            PubSubSubscriptionsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubSubscriptionsSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubSubscriptions>) const SWIFTEN_OVERRIDE;

        private:


        private:
            PayloadSerializerCollection* serializers;
    };
}
