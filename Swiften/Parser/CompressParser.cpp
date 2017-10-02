/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/CompressParser.h>

namespace Swift {

CompressParser::CompressParser() : GenericElementParser<CompressRequest>(), currentDepth_(0), inMethod_(false) {
}

void CompressParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap&) {
    if (currentDepth_ == 1 && element == "method") {
        inMethod_ = true;
        currentText_ = "";
    }
    ++currentDepth_;
}

void CompressParser::handleEndElement(const std::string&, const std::string&) {
    --currentDepth_;
    if (currentDepth_ == 1 && inMethod_) {
        getElementGeneric()->setMethod(currentText_);
        inMethod_ = false;
    }
}

void CompressParser::handleCharacterData(const std::string& data) {
    currentText_ += data;
}

}
