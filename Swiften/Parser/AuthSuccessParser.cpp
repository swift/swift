/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/AuthSuccessParser.h>

#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

AuthSuccessParser::AuthSuccessParser() : GenericElementParser<AuthSuccess>(), depth(0) {
}

void AuthSuccessParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    ++depth;
}

void AuthSuccessParser::handleEndElement(const std::string&, const std::string&) {
    --depth;
    if (depth == 0) {
        getElementGeneric()->setValue(Base64::decode(text));
    }
}

void AuthSuccessParser::handleCharacterData(const std::string& text) {
    this->text += text;
}

}
