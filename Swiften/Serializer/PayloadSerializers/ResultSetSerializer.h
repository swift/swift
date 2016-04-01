/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/ResultSet.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API ResultSetSerializer : public GenericPayloadSerializer<ResultSet> {
        public:
            ResultSetSerializer();
            virtual ~ResultSetSerializer();

            virtual std::string serializePayload(std::shared_ptr<ResultSet>) const SWIFTEN_OVERRIDE;
    };
}
