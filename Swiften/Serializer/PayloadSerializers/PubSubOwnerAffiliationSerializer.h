/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerAffiliation.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerAffiliationSerializer : public GenericPayloadSerializer<PubSubOwnerAffiliation> {
        public:
            PubSubOwnerAffiliationSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerAffiliationSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerAffiliation>) const override;

        private:
            static std::string serializeType(PubSubOwnerAffiliation::Type);
    };
}
