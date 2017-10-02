/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PriorityParser.h>

#include <boost/lexical_cast.hpp>

namespace Swift {

PriorityParser::PriorityParser() : level_(0) {
}

void PriorityParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    ++level_;
}

void PriorityParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
    if (level_ == 0) {
        int priority = 0;
        try {
            priority = boost::lexical_cast<int>(text_);
        }
        catch (boost::bad_lexical_cast&) {
        }
        getPayloadInternal()->setPriority(priority);
    }
}

void PriorityParser::handleCharacterData(const std::string& data) {
    text_ += data;
}

}
