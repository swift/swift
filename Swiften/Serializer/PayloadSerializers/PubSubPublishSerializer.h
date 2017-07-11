/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubPublish.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubPublishSerializer : public GenericPayloadSerializer<PubSubPublish> {
        public:
            PubSubPublishSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubPublishSerializer() SWIFTEN_OVERRIDE;

            virtual std::string serializePayload(std::shared_ptr<PubSubPublish>) const SWIFTEN_OVERRIDE;

        private:
            PayloadSerializerCollection* serializers;
    };
}
