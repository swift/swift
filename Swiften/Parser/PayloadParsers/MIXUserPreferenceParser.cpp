/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXUserPreferenceParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

namespace Swift {

MIXUserPreferenceParser::MIXUserPreferenceParser() : level_(0) {
}

MIXUserPreferenceParser::~MIXUserPreferenceParser() {
}

void MIXUserPreferenceParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {


    if (level_ == 1) {
        if (element == "x" && ns == "jabber:x:data") {
            currentPayloadParser_ = std::make_shared<FormParser>();
        }
    }

    if (level_ >= 1 && currentPayloadParser_) {
        currentPayloadParser_->handleStartElement(element, ns, attributes);
    }
    ++level_;
}

void MIXUserPreferenceParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level_;
    if (currentPayloadParser_) {
        if (level_ >= 1) {
            currentPayloadParser_->handleEndElement(element, ns);
        }

        if (level_ == 1) {
            if (element == "x" && ns == "jabber:x:data") {
                getPayloadInternal()->setData(std::dynamic_pointer_cast<Form>(currentPayloadParser_->getPayload()));
            }
            currentPayloadParser_.reset();
        }
    }
}

void MIXUserPreferenceParser::handleCharacterData(const std::string& data) {
    if (level_ > 1 && currentPayloadParser_) {
        currentPayloadParser_->handleCharacterData(data);
    }
}
}
