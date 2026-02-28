/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/BodyParser.h>

namespace Swift {

BodyParser::BodyParser() : level_(0) {
}

void BodyParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    ++level_;
}

void BodyParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
    if (level_ == 0) {
        getPayloadInternal()->setText(text_);
    }
}

void BodyParser::handleCharacterData(const std::string& data) {
    text_ += data;
}

}
