/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubOwnerRedirect.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubOwnerRedirectSerializer : public GenericPayloadSerializer<PubSubOwnerRedirect> {
        public:
            PubSubOwnerRedirectSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubOwnerRedirectSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubOwnerRedirect>) const override;
    };
}
