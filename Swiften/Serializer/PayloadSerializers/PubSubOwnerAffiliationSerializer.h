/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerAffiliation.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerAffiliationSerializer : public GenericPayloadSerializer<PubSubOwnerAffiliation> {
        public:
            PubSubOwnerAffiliationSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerAffiliationSerializer();

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerAffiliation>) const SWIFTEN_OVERRIDE;

        private:
            static std::string serializeType(PubSubOwnerAffiliation::Type);

        private:
            PayloadSerializerCollection* serializers;
    };
}
