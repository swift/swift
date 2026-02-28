/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/UserTune.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API UserTuneSerializer : public GenericPayloadSerializer<UserTune> {
        public:
            UserTuneSerializer(PayloadSerializerCollection* serializers);
            virtual ~UserTuneSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<UserTune>) const override;
    };
}
