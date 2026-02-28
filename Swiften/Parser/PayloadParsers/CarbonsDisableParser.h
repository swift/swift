/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsDisable.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API CarbonsDisableParser : public GenericPayloadParser<CarbonsDisable> {
        public:
            CarbonsDisableParser();
            virtual ~CarbonsDisableParser();

            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&);
            virtual void handleEndElement(const std::string&, const std::string&);
            virtual void handleCharacterData(const std::string&);
    };
}
