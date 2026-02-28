/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StanzaAck.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API StanzaAckSerializer : public GenericElementSerializer<StanzaAck> {
        public:
            StanzaAckSerializer() : GenericElementSerializer<StanzaAck>() {
            }

            virtual SafeByteArray serialize(std::shared_ptr<ToplevelElement> element) const {
                StanzaAck::ref stanzaAck(std::dynamic_pointer_cast<StanzaAck>(element));
                assert(stanzaAck->isValid());
                XMLElement result("a", "urn:xmpp:sm:2");
                result.setAttribute("h", std::string(std::to_string(stanzaAck->getHandledStanzasCount())));
                return createSafeByteArray(result.serialize());
            }
    };
}
