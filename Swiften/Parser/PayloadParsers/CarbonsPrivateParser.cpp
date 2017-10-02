/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/CarbonsPrivateParser.h>

namespace Swift {

    CarbonsPrivateParser::CarbonsPrivateParser() : GenericPayloadParser<CarbonsPrivate>() {
    }

    CarbonsPrivateParser::~CarbonsPrivateParser() {
    }

    void CarbonsPrivateParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    }

    void CarbonsPrivateParser::handleEndElement(const std::string&, const std::string&) {
    }

    void CarbonsPrivateParser::handleCharacterData(const std::string&) {
    }

}
