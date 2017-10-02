/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MAMFin.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API MAMFinSerializer : public GenericPayloadSerializer<MAMFin> {
        public:
            MAMFinSerializer();
            virtual ~MAMFinSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<MAMFin>) const override;
    };
}
