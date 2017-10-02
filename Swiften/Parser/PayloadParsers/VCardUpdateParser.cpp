/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/VCardUpdateParser.h>

namespace Swift {

VCardUpdateParser::VCardUpdateParser() : level_(TopLevel) {
}

void VCardUpdateParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    if (level_ == PayloadLevel) {
        currentText_ = "";
    }
    ++level_;
}

void VCardUpdateParser::handleEndElement(const std::string& element, const std::string&) {
    --level_;
    if (level_ == PayloadLevel && element == "photo") {
        getPayloadInternal()->setPhotoHash(currentText_);
    }
}

void VCardUpdateParser::handleCharacterData(const std::string& text) {
    currentText_ += text;
}

}
