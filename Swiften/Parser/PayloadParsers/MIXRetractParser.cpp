/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXRetractParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

namespace Swift {

MIXRetractParser::MIXRetractParser() : level_(0) {
}

MIXRetractParser::~MIXRetractParser() {
}

void MIXRetractParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
    if (level_ == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("id")) {
            getPayloadInternal()->setMessageID(*attributeValue);
        }
    }
    ++level_;
}

void MIXRetractParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
}

void MIXRetractParser::handleCharacterData(const std::string&) {
}

}
