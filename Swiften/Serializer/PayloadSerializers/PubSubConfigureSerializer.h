/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubConfigure.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubConfigureSerializer : public GenericPayloadSerializer<PubSubConfigure> {
        public:
            PubSubConfigureSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubConfigureSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubConfigure>) const SWIFTEN_OVERRIDE;
    };
}
