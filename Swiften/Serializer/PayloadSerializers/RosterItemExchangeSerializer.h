/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterItemExchangePayload.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API RosterItemExchangeSerializer : public GenericPayloadSerializer<RosterItemExchangePayload> {
        public:
            RosterItemExchangeSerializer();

            virtual std::string serializePayload(std::shared_ptr<RosterItemExchangePayload>)  const;
    };
}
