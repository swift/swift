/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerAffiliations.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerAffiliationsSerializer : public GenericPayloadSerializer<PubSubOwnerAffiliations> {
        public:
            PubSubOwnerAffiliationsSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerAffiliationsSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerAffiliations>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
