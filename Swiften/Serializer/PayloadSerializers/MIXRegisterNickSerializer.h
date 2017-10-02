/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXRegisterNick.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API MIXRegisterNickSerializer : public GenericPayloadSerializer<MIXRegisterNick> {
        public:
            MIXRegisterNickSerializer();

            virtual std::string serializePayload(std::shared_ptr<MIXRegisterNick> payload) const override;
    };
}
