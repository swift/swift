/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubAffiliation.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubAffiliationSerializer : public GenericPayloadSerializer<PubSubAffiliation> {
        public:
            PubSubAffiliationSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubAffiliationSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubAffiliation>) const override;

        private:
            static std::string serializeType(PubSubAffiliation::Type);
    };
}
