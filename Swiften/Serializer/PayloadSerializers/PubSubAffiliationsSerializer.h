/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubAffiliations.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubAffiliationsSerializer : public GenericPayloadSerializer<PubSubAffiliations> {
        public:
            PubSubAffiliationsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubAffiliationsSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubAffiliations>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
