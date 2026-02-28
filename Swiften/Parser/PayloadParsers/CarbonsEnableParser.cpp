/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/CarbonsEnableParser.h>

namespace Swift {

    CarbonsEnableParser::CarbonsEnableParser() : GenericPayloadParser<CarbonsEnable>() {
    }

    CarbonsEnableParser::~CarbonsEnableParser() {
    }

    void CarbonsEnableParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    }

    void CarbonsEnableParser::handleEndElement(const std::string&, const std::string&) {
    }

    void CarbonsEnableParser::handleCharacterData(const std::string&) {
    }

}
