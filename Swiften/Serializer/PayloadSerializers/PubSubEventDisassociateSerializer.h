/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventDisassociate.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubEventDisassociateSerializer : public GenericPayloadSerializer<PubSubEventDisassociate> {
        public:
            PubSubEventDisassociateSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubEventDisassociateSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubEventDisassociate>) const override;
    };
}
