/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerDefault.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerDefaultSerializer : public GenericPayloadSerializer<PubSubOwnerDefault> {
        public:
            PubSubOwnerDefaultSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerDefaultSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerDefault>) const override;
    };
}
