/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXSubscribeParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>

using namespace Swift;

MIXSubscribeParser::MIXSubscribeParser() : level_(0) {
}

MIXSubscribeParser::~MIXSubscribeParser() {
}

void MIXSubscribeParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
    if (level_ == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
    }
    ++level_;
}

void MIXSubscribeParser::handleEndElement(const std::string& , const std::string& ) {
    --level_;
}

void MIXSubscribeParser::handleCharacterData(const std::string& ) {
}
