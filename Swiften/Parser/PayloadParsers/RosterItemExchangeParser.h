/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterItemExchangePayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API RosterItemExchangeParser : public GenericPayloadParser<RosterItemExchangePayload> {
        public:
            RosterItemExchangeParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            enum Level {
                TopLevel = 0,
                PayloadLevel = 1,
                ItemLevel = 2
            };
            int level_;
            bool inItem_;
            RosterItemExchangePayload::Item currentItem_;
            std::string currentText_;
    };
}
