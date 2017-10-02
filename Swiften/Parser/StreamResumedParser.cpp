/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/StreamResumedParser.h>

#include <boost/lexical_cast.hpp>

using namespace Swift;

StreamResumedParser::StreamResumedParser() : level(TopLevel) {
}

StreamResumedParser::~StreamResumedParser() {
}

void StreamResumedParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
    if (level == TopLevel) {
        boost::optional<std::string> handledStanzasCount = attributes.getAttributeValue("h");
        if (handledStanzasCount) {
            try {
                getElementGeneric()->setHandledStanzasCount(boost::lexical_cast<unsigned int>(*handledStanzasCount));
            }
            catch (const boost::bad_lexical_cast &) {
            }
        }
        getElementGeneric()->setResumeID(attributes.getAttribute("previd"));
    }
    ++level;
}

void StreamResumedParser::handleEndElement(const std::string&, const std::string&) {
    --level;
}
