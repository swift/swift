/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubEventCollection.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventCollectionSerializer : public GenericPayloadSerializer<PubSubEventCollection> {
        public:
            PubSubEventCollectionSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventCollectionSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubEventCollection>) const SWIFTEN_OVERRIDE;

        private:


        private:
            PayloadSerializerCollection* serializers;
    };
}
