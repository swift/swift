/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API ForwardedSerializer : public GenericPayloadSerializer<Forwarded> {
        public:
            ForwardedSerializer(PayloadSerializerCollection* serializers);
            virtual ~ForwardedSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<Forwarded>) const override;

        private:
            PayloadSerializerCollection* serializers_;
    };
}
