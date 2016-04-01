/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API VCardSerializer : public GenericPayloadSerializer<VCard> {
        public:
            VCardSerializer();

            virtual std::string serializePayload(std::shared_ptr<VCard>)  const;
    };
}
