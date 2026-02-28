/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/CarbonsDisableParser.h>

namespace Swift {

    CarbonsDisableParser::CarbonsDisableParser() : GenericPayloadParser<CarbonsDisable>() {
    }

    CarbonsDisableParser::~CarbonsDisableParser() {
    }

    void CarbonsDisableParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    }

    void CarbonsDisableParser::handleEndElement(const std::string&, const std::string&) {
    }

    void CarbonsDisableParser::handleCharacterData(const std::string&) {
    }

}
