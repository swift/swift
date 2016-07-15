/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubEventParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventCollectionParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventConfigurationParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventDeleteParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventItemsParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventPurgeParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventSubscriptionParser.h>

using namespace Swift;

PubSubEventParser::PubSubEventParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubEventParser::~PubSubEventParser() {
}

void PubSubEventParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {


    if (level == 1) {
        if (element == "items" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventItemsParser>(parsers);
        }
        if (element == "collection" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventCollectionParser>(parsers);
        }
        if (element == "purge" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventPurgeParser>(parsers);
        }
        if (element == "configuration" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventConfigurationParser>(parsers);
        }
        if (element == "delete" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventDeleteParser>(parsers);
        }
        if (element == "subscription" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventSubscriptionParser>(parsers);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubEventParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (currentPayloadParser) {
                getPayloadInternal()->setPayload(std::dynamic_pointer_cast<PubSubEventPayload>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubEventParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
