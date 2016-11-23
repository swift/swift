/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/XML/XMLElement.h>

#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

XMLElement::XMLElement(const std::string& tag, const std::string& xmlns, const std::string& text) : tag_(tag) {
    if (!xmlns.empty()) {
        setAttribute("xmlns", xmlns);
    }
    if (!text.empty()) {
        addNode(XMLTextNode::ref(new XMLTextNode(text)));
    }
}

std::string XMLElement::serialize() {
    std::string result;
    result += "<" + tag_;
    for (const auto& p : attributes_) {
        result += " " + p.first + "=\"" + p.second + "\"";
    }

    if (!childNodes_.empty()) {
        result += ">";
        for (auto& node : childNodes_) {
            result += node->serialize();
        }
        result += "</" + tag_ + ">";
    }
    else {
        result += "/>";
    }
    return result;
}

void XMLElement::setAttribute(const std::string& attribute, const std::string& value) {
    std::string escapedValue(value);
    String::replaceAll(escapedValue, '&', "&amp;");
    String::replaceAll(escapedValue, '<', "&lt;");
    String::replaceAll(escapedValue, '>', "&gt;");
    String::replaceAll(escapedValue, '\'', "&apos;");
    String::replaceAll(escapedValue, '"', "&quot;");
    attributes_[attribute] = escapedValue;
}

void XMLElement::addNode(std::shared_ptr<XMLNode> node) {
    if (node) {
        childNodes_.push_back(node);
    }
}

}
