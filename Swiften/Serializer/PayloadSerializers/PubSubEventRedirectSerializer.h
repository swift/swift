/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventRedirect.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventRedirectSerializer : public GenericPayloadSerializer<PubSubEventRedirect> {
        public:
            PubSubEventRedirectSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventRedirectSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventRedirect>) const override;
    };
}
