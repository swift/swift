/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubCreate.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubCreateSerializer : public GenericPayloadSerializer<PubSubCreate> {
        public:
            PubSubCreateSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubCreateSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubCreate>) const override;
    };
}
