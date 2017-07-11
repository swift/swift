/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MAMQuery.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API MAMQuerySerializer : public GenericPayloadSerializer<MAMQuery> {
        public:
            MAMQuerySerializer();
            virtual ~MAMQuerySerializer() SWIFTEN_OVERRIDE;

            virtual std::string serializePayload(std::shared_ptr<MAMQuery>) const SWIFTEN_OVERRIDE;
    };
}
