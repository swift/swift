/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOptions.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOptionsSerializer : public GenericPayloadSerializer<PubSubOptions> {
        public:
            PubSubOptionsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOptionsSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOptions>) const override;
    };
}
