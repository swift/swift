/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsEnable.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API CarbonsEnableSerializer : public GenericPayloadSerializer<CarbonsEnable> {
        public:
            CarbonsEnableSerializer();
            virtual ~CarbonsEnableSerializer();

            virtual std::string serializePayload(std::shared_ptr<CarbonsEnable>)    const;
    };
}
