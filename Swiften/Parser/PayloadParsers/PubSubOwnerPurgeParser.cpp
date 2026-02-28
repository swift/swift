/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubOwnerPurgeParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>

using namespace Swift;

PubSubOwnerPurgeParser::PubSubOwnerPurgeParser(PayloadParserFactoryCollection* /*parsers*/) : level(0) {
}

PubSubOwnerPurgeParser::~PubSubOwnerPurgeParser() {
}

void PubSubOwnerPurgeParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubOwnerPurgeParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {

            currentPayloadParser.reset();
        }
    }
}

void PubSubOwnerPurgeParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
