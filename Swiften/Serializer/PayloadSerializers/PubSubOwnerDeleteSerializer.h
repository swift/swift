/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerDelete.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerDeleteSerializer : public GenericPayloadSerializer<PubSubOwnerDelete> {
        public:
            PubSubOwnerDeleteSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerDeleteSerializer() SWIFTEN_OVERRIDE;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerDelete>) const SWIFTEN_OVERRIDE;

        private:
            PayloadSerializerCollection* serializers;
    };
}
