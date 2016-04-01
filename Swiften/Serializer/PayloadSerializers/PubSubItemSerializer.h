/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubItemSerializer : public GenericPayloadSerializer<PubSubItem> {
        public:
            PubSubItemSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubItemSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubItem>) const SWIFTEN_OVERRIDE;

        private:


        private:
            PayloadSerializerCollection* serializers;
    };
}
