/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API RosterSerializer : public GenericPayloadSerializer<RosterPayload> {
        public:
            RosterSerializer();

            virtual std::string serializePayload(std::shared_ptr<RosterPayload>)  const;
    };
}
