/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventDelete.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventDeleteSerializer : public GenericPayloadSerializer<PubSubEventDelete> {
        public:
            PubSubEventDeleteSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventDeleteSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventDelete>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
