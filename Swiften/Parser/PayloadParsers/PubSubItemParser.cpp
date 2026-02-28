/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubItemParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>

using namespace Swift;

PubSubItemParser::PubSubItemParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubItemParser::~PubSubItemParser() {
}

void PubSubItemParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("id")) {
            getPayloadInternal()->setID(*attributeValue);
        }
    }

    if (level == 1) {
        if (PayloadParserFactory* factory = parsers->getPayloadParserFactory(element, ns, attributes)) {
            currentPayloadParser.reset(factory->createPayloadParser());
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubItemParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            getPayloadInternal()->addData(currentPayloadParser->getPayload());
            currentPayloadParser.reset();
        }
    }
}

void PubSubItemParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
