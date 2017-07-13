/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEvent.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventSerializer : public GenericPayloadSerializer<PubSubEvent> {
        public:
            PubSubEventSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEvent>) const override;

        private:
            std::vector< std::shared_ptr<PayloadSerializer> > pubsubSerializers;
    };
}
