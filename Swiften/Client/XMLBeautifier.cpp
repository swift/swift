/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/XMLBeautifier.h>

#include <sstream>
#include <stack>
#include <iostream>

#include <Swiften/Base/Log.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>

namespace Swift {

XMLBeautifier::XMLBeautifier(bool indention, bool coloring) : doIndention_(indention), doColoring_(coloring), factory_(new PlatformXMLParserFactory()) {
}


std::string XMLBeautifier::beautify(const std::string &text) {
    wasReset_ = false;
    if (!parser_) {
        reset();
    }
    buffer_.str(std::string());
    if (!parser_->parse(text)) {
        reset();
        parser_->parse(text);
    }
    return buffer_.str();
}

bool XMLBeautifier::wasReset() const {
    return wasReset_;
}

int XMLBeautifier::getLevel() const {
    return intLevel_;
}

void XMLBeautifier::indent() {
    for (int i = 0; i < intLevel_; ++i) {
        buffer_ << " ";
    }
}

void XMLBeautifier::reset() {
    parser_ = factory_->createXMLParser(this);
    intLevel_ = 0;
    lastWasStepDown_ = false;
    std::stack<std::string>().swap(parentNSs_);
    buffer_.str(std::string());
    wasReset_ = true;
}

// all bold but reset
// static const char colorBlue[] = "\x1b[01;34m";
static const char colorCyan[] = "\x1b[01;36m";
static const char colorGreen[] = "\x1b[01;32m";
// static const char colorMagenta[] = "\x1b[01;35m";
static const char colorRed[] = "\x1b[01;31m";
static const char colorReset[] = "\x1b[0m";
static const char colorYellow[] = "\x1b[01;33m";



std::string XMLBeautifier::styleTag(const std::string& text) const {
    std::string result;
    result += colorYellow;
    result += text;
    result += colorReset;
    return result;
}

std::string XMLBeautifier::styleNamespace(const std::string& text) const {
    std::string result;
    result += colorRed;
    result += text;
    result += colorReset;
    return result;
}

std::string XMLBeautifier::styleAttribute(const std::string& text) const {
    std::string result;
    result += colorGreen;
    result += text;
    result += colorReset;
    return result;
}
std::string XMLBeautifier::styleValue(const std::string& text) const {
    std::string result;
    result += colorCyan;
    result += text;
    result += colorReset;
    return result;
}

void XMLBeautifier::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (doIndention_) {
        if (intLevel_) buffer_ << std::endl;
    }
    indent();
    buffer_ << "<" << (doColoring_ ? styleTag(element) : element);
    if (!ns.empty() && (!parentNSs_.empty() && parentNSs_.top() != ns)) {
        buffer_ << " ";
        buffer_ << (doColoring_ ? styleAttribute("xmlns") : "xmlns");
        buffer_ << "=";
        buffer_ << "\"" << (doColoring_ ? styleNamespace(ns) : ns) << "\"";
    }
    if (!attributes.getEntries().empty()) {
        for (const auto& entry : attributes.getEntries()) {
            buffer_ << " ";
            buffer_ << (doColoring_ ? styleAttribute(entry.getAttribute().getName()) : entry.getAttribute().getName());
            buffer_ << "=";
            buffer_ << "\"" << (doColoring_ ? styleValue(entry.getValue()) : entry.getValue()) << "\"";
        }
    }
    buffer_ << ">";
    ++intLevel_;
    lastWasStepDown_ = false;
    parentNSs_.push(ns);
}

void XMLBeautifier::handleEndElement(const std::string& element, const std::string& /* ns */) {
    --intLevel_;
    parentNSs_.pop();
    if (/*hadCDATA.top() ||*/ lastWasStepDown_) {
        if (doIndention_) {
            buffer_ << std::endl;
        }
        indent();
    }
    buffer_ << "</" << (doColoring_ ? styleTag(element) : element) << ">";
    lastWasStepDown_ = true;
}

void XMLBeautifier::handleCharacterData(const std::string& data) {
    buffer_ << data;
    lastWasStepDown_ = false;
}

}
