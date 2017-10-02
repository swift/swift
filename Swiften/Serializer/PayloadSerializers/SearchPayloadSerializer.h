/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/SearchPayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API SearchPayloadSerializer : public GenericPayloadSerializer<SearchPayload> {
        public:
            SearchPayloadSerializer();

            virtual std::string serializePayload(std::shared_ptr<SearchPayload>)  const;
    };
}
