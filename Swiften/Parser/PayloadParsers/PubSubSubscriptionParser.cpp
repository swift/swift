/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubSubscriptionParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/EnumParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubSubscribeOptionsParser.h>

using namespace Swift;

PubSubSubscriptionParser::PubSubSubscriptionParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubSubscriptionParser::~PubSubSubscriptionParser() {
}

void PubSubSubscriptionParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("subid")) {
            getPayloadInternal()->setSubscriptionID(*attributeValue);
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("jid")) {
            if (boost::optional<JID> jid = JID::parse(*attributeValue)) {
                getPayloadInternal()->setJID(*jid);
            }
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("subscription")) {
            if (boost::optional<PubSubSubscription::SubscriptionType> value = EnumParser<PubSubSubscription::SubscriptionType>()(PubSubSubscription::None, "none")(PubSubSubscription::Pending, "pending")(PubSubSubscription::Subscribed, "subscribed")(PubSubSubscription::Unconfigured, "unconfigured").parse(*attributeValue)) {
                getPayloadInternal()->setSubscription(*value);
            }
        }
    }

    if (level == 1) {
        if (element == "subscribe-options" && ns == "http://jabber.org/protocol/pubsub") {
            currentPayloadParser = std::make_shared<PubSubSubscribeOptionsParser>(parsers);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubSubscriptionParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (element == "subscribe-options" && ns == "http://jabber.org/protocol/pubsub") {
                getPayloadInternal()->setOptions(std::dynamic_pointer_cast<PubSubSubscribeOptions>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubSubscriptionParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
