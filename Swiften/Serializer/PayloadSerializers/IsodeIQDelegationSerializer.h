/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/IsodeIQDelegation.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API IsodeIQDelegationSerializer : public GenericPayloadSerializer<IsodeIQDelegation> {
        public:
            IsodeIQDelegationSerializer(PayloadSerializerCollection* serializers);
            virtual ~IsodeIQDelegationSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<IsodeIQDelegation>) const override;

        private:
            PayloadSerializerCollection* serializers;
    };
}
