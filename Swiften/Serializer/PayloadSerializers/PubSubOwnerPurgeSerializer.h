/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerPurge.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerPurgeSerializer : public GenericPayloadSerializer<PubSubOwnerPurge> {
        public:
            PubSubOwnerPurgeSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerPurgeSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerPurge>) const override;
    };
}
