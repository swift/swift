/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MUCAdminPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API MUCAdminPayloadSerializer : public GenericPayloadSerializer<MUCAdminPayload> {
        public:
            MUCAdminPayloadSerializer();
            std::string affiliationToString(MUCOccupant::Affiliation affiliation) const;
            std::string roleToString(MUCOccupant::Role role) const;

            virtual std::string serializePayload(std::shared_ptr<MUCAdminPayload> version)  const;
    };
}

