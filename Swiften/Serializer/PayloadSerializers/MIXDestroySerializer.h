/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXDestroy.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {

    class SWIFTEN_API MIXDestroySerializer : public GenericPayloadSerializer<MIXDestroy> {
        public:
            MIXDestroySerializer();
            virtual ~MIXDestroySerializer() override;

            virtual std::string serializePayload(std::shared_ptr<MIXDestroy>) const override;
    };
}
