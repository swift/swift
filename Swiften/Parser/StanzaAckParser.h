/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StanzaAck.h>
#include <Swiften/Parser/GenericElementParser.h>

namespace Swift {
    class SWIFTEN_API StanzaAckParser : public GenericElementParser<StanzaAck> {
        public:
            StanzaAckParser();

            virtual void handleStartElement(const std::string&, const std::string& ns, const AttributeMap&);
            virtual void handleEndElement(const std::string&, const std::string& ns);

        private:
            int depth;
    };
}
