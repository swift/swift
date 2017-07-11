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
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MIXUserPreference.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {

    class SWIFTEN_API MIXUserPreferenceSerializer : public GenericPayloadSerializer<MIXUserPreference> {
        public:
            MIXUserPreferenceSerializer();
            virtual ~MIXUserPreferenceSerializer() SWIFTEN_OVERRIDE;

            virtual std::string serializePayload(std::shared_ptr<MIXUserPreference>) const SWIFTEN_OVERRIDE;
    };
}
