/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubSubscribeParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>

using namespace Swift;

PubSubSubscribeParser::PubSubSubscribeParser(PayloadParserFactoryCollection* /*parsers*/) : level(0) {
}

PubSubSubscribeParser::~PubSubSubscribeParser() {
}

void PubSubSubscribeParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("jid")) {
            if (boost::optional<JID> jid = JID::parse(*attributeValue)) {
                getPayloadInternal()->setJID(*jid);
            }
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubSubscribeParser::handleEndElement(const std::string& element, const std::string& ns) {
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

void PubSubSubscribeParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
