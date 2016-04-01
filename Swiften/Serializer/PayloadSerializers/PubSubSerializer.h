/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubSerializer : public GenericPayloadSerializer<PubSub> {
        public:
            PubSubSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSub>) const SWIFTEN_OVERRIDE;

        private:
            std::vector< std::shared_ptr<PayloadSerializer> > pubsubSerializers;
            PayloadSerializerCollection* serializers;
    };
}
