/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/SerializingParser.h>

#include <cassert>
#include <memory>

#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

SerializingParser::SerializingParser() {
}

void SerializingParser::handleStartElement(const std::string& tag, const std::string&  ns, const AttributeMap& attributes) {
    std::shared_ptr<XMLElement> element = std::make_shared<XMLElement>(tag, ns);
    // FIXME: Ignoring attribute namespace
    for (const auto& e : attributes.getEntries()) {
        element->setAttribute(e.getAttribute().getName(), e.getValue());
    }

    if (elementStack_.empty()) {
        rootElement_ = element;
    }
    else {
        (*(elementStack_.end() - 1))->addNode(element);
    }
    elementStack_.push_back(element);
}

void SerializingParser::handleEndElement(const std::string&, const std::string&) {
    assert(!elementStack_.empty());
    elementStack_.pop_back();
}

void SerializingParser::handleCharacterData(const std::string& data) {
    if (!elementStack_.empty()) {
        (*(elementStack_.end()-1))->addNode(std::make_shared<XMLTextNode>(data));
    }
}

std::string SerializingParser::getResult() const {
    return (rootElement_ ? rootElement_->serialize() : "");
}

}
