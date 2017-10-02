/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Last.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API LastSerializer : public GenericPayloadSerializer<Last> {
        public:
            LastSerializer() : GenericPayloadSerializer<Last>() {}

            virtual std::string serializePayload(std::shared_ptr<Last> last)  const {
                return "<query xmlns='jabber:iq:last' seconds='" + boost::lexical_cast<std::string>(last->getSeconds()) + "'/>";
            }
    };
}
