/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerDefault.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerDefaultSerializer : public GenericPayloadSerializer<PubSubOwnerDefault> {
        public:
            PubSubOwnerDefaultSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerDefaultSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerDefault>) const SWIFTEN_OVERRIDE;
    };
}
