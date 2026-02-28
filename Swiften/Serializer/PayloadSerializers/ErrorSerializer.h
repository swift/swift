/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API ErrorSerializer : public GenericPayloadSerializer<ErrorPayload> {
        public:
            ErrorSerializer(PayloadSerializerCollection* serializers);

            virtual std::string serializePayload(std::shared_ptr<ErrorPayload> error)  const;

        private:
            PayloadSerializerCollection* serializers;
    };
}
