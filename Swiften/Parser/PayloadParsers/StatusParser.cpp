/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/StatusParser.h>

namespace Swift {

StatusParser::StatusParser() : level_(0) {
}

void StatusParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    ++level_;
}

void StatusParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
    if (level_ == 0) {
        getPayloadInternal()->setText(text_);
    }
}

void StatusParser::handleCharacterData(const std::string& data) {
    text_ += data;
}

}
