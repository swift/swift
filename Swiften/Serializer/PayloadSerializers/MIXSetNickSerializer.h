/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MIXSetNick.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API MIXSetNickSerializer : public GenericPayloadSerializer<MIXSetNick> {
        public:
            MIXSetNickSerializer();

            virtual std::string serializePayload(std::shared_ptr<MIXSetNick> payload) const SWIFTEN_OVERRIDE ;
    };
}
