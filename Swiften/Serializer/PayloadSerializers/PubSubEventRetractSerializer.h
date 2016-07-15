/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubEventRetract.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventRetractSerializer : public GenericPayloadSerializer<PubSubEventRetract> {
        public:
            PubSubEventRetractSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventRetractSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubEventRetract>) const SWIFTEN_OVERRIDE;
    };
}
