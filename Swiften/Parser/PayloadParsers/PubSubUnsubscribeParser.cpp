/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubUnsubscribeParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>

using namespace Swift;

PubSubUnsubscribeParser::PubSubUnsubscribeParser(PayloadParserFactoryCollection* /*parsers*/) : level(0) {
}

PubSubUnsubscribeParser::~PubSubUnsubscribeParser() {
}

void PubSubUnsubscribeParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("jid")) {
            if (boost::optional<JID> jid = JID::parse(*attributeValue)) {
                getPayloadInternal()->setJID(*jid);
            }
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("subid")) {
            getPayloadInternal()->setSubscriptionID(*attributeValue);
        }
    }



    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubUnsubscribeParser::handleEndElement(const std::string& element, const std::string& ns) {
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

void PubSubUnsubscribeParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
