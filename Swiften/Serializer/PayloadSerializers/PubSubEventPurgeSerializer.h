/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubEventPurge.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventPurgeSerializer : public GenericPayloadSerializer<PubSubEventPurge> {
        public:
            PubSubEventPurgeSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventPurgeSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubEventPurge>) const SWIFTEN_OVERRIDE;
    };
}
