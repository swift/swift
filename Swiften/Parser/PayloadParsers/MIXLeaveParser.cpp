/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXLeaveParser.h>

#include <boost/optional.hpp>

namespace Swift {

MIXLeaveParser::MIXLeaveParser() : level_(0) {
}

MIXLeaveParser::~MIXLeaveParser() {
}

void MIXLeaveParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
    if (level_ == 0) {
        if (auto attributeValue = attributes.getAttributeValue("channel")) {
            if (auto jid = JID::parse(*attributeValue)) {
                getPayloadInternal()->setChannel(*jid);
            }
        }
    }
    ++level_;
}

void MIXLeaveParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
}

void MIXLeaveParser::handleCharacterData(const std::string&) {

}
}
