/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/UserLocation.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API UserLocationSerializer : public GenericPayloadSerializer<UserLocation> {
        public:
            UserLocationSerializer(PayloadSerializerCollection* serializers);
            virtual ~UserLocationSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<UserLocation>) const override;
    };
}
