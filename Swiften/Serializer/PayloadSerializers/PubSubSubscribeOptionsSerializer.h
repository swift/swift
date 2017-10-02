/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubSubscribeOptions.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubSubscribeOptionsSerializer : public GenericPayloadSerializer<PubSubSubscribeOptions> {
        public:
            PubSubSubscribeOptionsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubSubscribeOptionsSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubSubscribeOptions>) const override;
    };
}
