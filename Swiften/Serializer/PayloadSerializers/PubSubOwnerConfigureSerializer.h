/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerConfigure.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerConfigureSerializer : public GenericPayloadSerializer<PubSubOwnerConfigure> {
        public:
            PubSubOwnerConfigureSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerConfigureSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerConfigure>) const override;
    };
}
