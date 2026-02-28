/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Replace.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {
    class SWIFTEN_API ReplaceSerializer : public GenericPayloadSerializer<Replace> {
        public:
            ReplaceSerializer() : GenericPayloadSerializer<Replace>() {}

            virtual std::string serializePayload(std::shared_ptr<Replace> replace) const {
                return "<replace id = '" + replace->getID() + "' xmlns='urn:xmpp:message-correct:0'/>";
            }
    };
}
