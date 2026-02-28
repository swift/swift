/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubSubscriptionsParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubSubscriptionParser.h>

using namespace Swift;

PubSubSubscriptionsParser::PubSubSubscriptionsParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubSubscriptionsParser::~PubSubSubscriptionsParser() {
}

void PubSubSubscriptionsParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
    }

    if (level == 1) {
        if (element == "subscription" && ns == "http://jabber.org/protocol/pubsub") {
            currentPayloadParser = std::make_shared<PubSubSubscriptionParser>(parsers);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubSubscriptionsParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (element == "subscription" && ns == "http://jabber.org/protocol/pubsub") {
                getPayloadInternal()->addSubscription(std::dynamic_pointer_cast<PubSubSubscription>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubSubscriptionsParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
