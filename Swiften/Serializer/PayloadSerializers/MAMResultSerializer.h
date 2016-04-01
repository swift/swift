/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MAMResult.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API MAMResultSerializer : public GenericPayloadSerializer<MAMResult> {
        public:
            MAMResultSerializer(PayloadSerializerCollection* serializers);
            virtual ~MAMResultSerializer();

            virtual std::string serializePayload(std::shared_ptr<MAMResult>) const SWIFTEN_OVERRIDE;

        private:
            PayloadSerializerCollection* serializers_;
    };
}
