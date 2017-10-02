/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubUnsubscribe.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubUnsubscribeSerializer : public GenericPayloadSerializer<PubSubUnsubscribe> {
        public:
            PubSubUnsubscribeSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubUnsubscribeSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubUnsubscribe>) const override;
    };
}
