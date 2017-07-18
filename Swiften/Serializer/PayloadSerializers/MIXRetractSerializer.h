/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXRetract.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {

    class SWIFTEN_API MIXRetractSerializer : public GenericPayloadSerializer<MIXRetract> {
        public:
            MIXRetractSerializer();
            virtual ~MIXRetractSerializer();

            virtual std::string serializePayload(std::shared_ptr<MIXRetract>) const override;
    };
}
