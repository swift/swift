/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/ResourceBindParser.h>

namespace Swift {

ResourceBindParser::ResourceBindParser() : level_(0), inJID_(false), inResource_(false) {
}

void ResourceBindParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap&) {
    if (level_ == 1) {
        text_ = "";
        if (element == "resource") {
            inResource_ = true;
        }
        if (element == "jid") {
            inJID_ = true;
        }
    }
    ++level_;
}

void ResourceBindParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
    if (level_ == 1) {
        if (inJID_) {
            getPayloadInternal()->setJID(JID(text_));
        }
        else if (inResource_) {
            getPayloadInternal()->setResource(text_);
        }
    }
}

void ResourceBindParser::handleCharacterData(const std::string& data) {
    text_ += data;
}

}
