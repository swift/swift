/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/CapsInfoParser.h>

#include <locale>

namespace Swift {

CapsInfoParser::CapsInfoParser() : level(0) {
}

void CapsInfoParser::handleStartElement(const std::string&, const std::string& /*ns*/, const AttributeMap& attributes) {
    if (level == 0) {
        getPayloadInternal()->setHash(attributes.getAttribute("hash"));
        getPayloadInternal()->setNode(attributes.getAttribute("node"));
        getPayloadInternal()->setVersion(attributes.getAttribute("ver"));
    }
    ++level;
}

void CapsInfoParser::handleEndElement(const std::string&, const std::string&) {
    --level;
}

void CapsInfoParser::handleCharacterData(const std::string&) {

}

}
