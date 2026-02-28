/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/ThreadParser.h>

#include <boost/optional.hpp>

namespace Swift {

ThreadParser::ThreadParser() : level_(0) {
}

ThreadParser::~ThreadParser() {
}

void ThreadParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
    ++level_;
    if (element == "thread") {
        getPayloadInternal()->setParent(attributes.getAttributeValue("parent").get_value_or(""));
    }
}

void ThreadParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
    if (level_ == 0) {
        getPayloadInternal()->setText(text_);
    }
}

void ThreadParser::handleCharacterData(const std::string& data) {
    if (level_ == 1) {
        text_ += data;
    }
}

}
