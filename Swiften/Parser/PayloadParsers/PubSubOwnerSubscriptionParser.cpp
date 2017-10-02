/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubOwnerSubscriptionParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/EnumParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>

using namespace Swift;

PubSubOwnerSubscriptionParser::PubSubOwnerSubscriptionParser(PayloadParserFactoryCollection* /*parsers*/) : level(0) {
}

PubSubOwnerSubscriptionParser::~PubSubOwnerSubscriptionParser() {
}

void PubSubOwnerSubscriptionParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("jid")) {
            if (boost::optional<JID> jid = JID::parse(*attributeValue)) {
                getPayloadInternal()->setJID(*jid);
            }
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("subscription")) {
            if (boost::optional<PubSubOwnerSubscription::SubscriptionType> value = EnumParser<PubSubOwnerSubscription::SubscriptionType>()(PubSubOwnerSubscription::None, "none")(PubSubOwnerSubscription::Pending, "pending")(PubSubOwnerSubscription::Subscribed, "subscribed")(PubSubOwnerSubscription::Unconfigured, "unconfigured").parse(*attributeValue)) {
                getPayloadInternal()->setSubscription(*value);
            }
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubOwnerSubscriptionParser::handleEndElement(const std::string& element, const std::string& ns) {
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

void PubSubOwnerSubscriptionParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
