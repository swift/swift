/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsPrivate.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API CarbonsPrivateParser : public GenericPayloadParser<CarbonsPrivate> {
        public:
            CarbonsPrivateParser();
            virtual ~CarbonsPrivateParser();
            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&);
            virtual void handleEndElement(const std::string&, const std::string&);
            virtual void handleCharacterData(const std::string&);
    };
}
