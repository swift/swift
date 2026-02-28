/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Idle.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API IdleSerializer : public GenericPayloadSerializer<Idle> {
        public:
            IdleSerializer() : GenericPayloadSerializer<Idle>() {}

            virtual std::string serializePayload(std::shared_ptr<Idle> idle)  const {
                return "<idle xmlns='urn:xmpp:idle:1' since='" + dateTimeToString(idle->getSince()) + "'/>";
            }
    };
}
