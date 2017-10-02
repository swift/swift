/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubRetract.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubRetractSerializer : public GenericPayloadSerializer<PubSubRetract> {
        public:
            PubSubRetractSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubRetractSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubRetract>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
