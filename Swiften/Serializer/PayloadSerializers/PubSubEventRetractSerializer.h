/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventRetract.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventRetractSerializer : public GenericPayloadSerializer<PubSubEventRetract> {
        public:
            PubSubEventRetractSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventRetractSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventRetract>) const override;
    };
}
