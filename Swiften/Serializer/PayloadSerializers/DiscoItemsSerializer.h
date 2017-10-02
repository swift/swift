/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API DiscoItemsSerializer : public GenericPayloadSerializer<DiscoItems> {
        public:
            DiscoItemsSerializer();

            virtual std::string serializePayload(std::shared_ptr<DiscoItems>)  const;
    };
}

