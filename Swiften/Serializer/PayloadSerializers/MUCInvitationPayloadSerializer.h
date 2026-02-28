/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API MUCInvitationPayloadSerializer : public GenericPayloadSerializer<MUCInvitationPayload> {
        public:
            MUCInvitationPayloadSerializer();

            virtual std::string serializePayload(std::shared_ptr<MUCInvitationPayload> version)  const;
    };
}

