/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubSubscribe.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubSubscribeSerializer : public GenericPayloadSerializer<PubSubSubscribe> {
        public:
            PubSubSubscribeSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubSubscribeSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubSubscribe>) const SWIFTEN_OVERRIDE;
    };
}
