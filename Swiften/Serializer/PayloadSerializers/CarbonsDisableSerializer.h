/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsDisable.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API CarbonsDisableSerializer : public GenericPayloadSerializer<CarbonsDisable> {
        public:
            CarbonsDisableSerializer();
            virtual ~CarbonsDisableSerializer();

            virtual std::string serializePayload(std::shared_ptr<CarbonsDisable>)    const;
    };
}
