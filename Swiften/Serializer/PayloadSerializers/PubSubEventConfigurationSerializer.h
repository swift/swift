/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventConfiguration.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventConfigurationSerializer : public GenericPayloadSerializer<PubSubEventConfiguration> {
        public:
            PubSubEventConfigurationSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventConfigurationSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventConfiguration>) const override;
    };
}
