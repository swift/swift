/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubOwnerPubSubParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubOwnerAffiliationsParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubOwnerConfigureParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubOwnerDefaultParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubOwnerDeleteParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubOwnerPurgeParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubOwnerSubscriptionsParser.h>

using namespace Swift;

PubSubOwnerPubSubParser::PubSubOwnerPubSubParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubOwnerPubSubParser::~PubSubOwnerPubSubParser() {
}

void PubSubOwnerPubSubParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {


    if (level == 1) {
        if (element == "configure" && ns == "http://jabber.org/protocol/pubsub#owner") {
            currentPayloadParser = std::make_shared<PubSubOwnerConfigureParser>(parsers);
        }
        if (element == "subscriptions" && ns == "http://jabber.org/protocol/pubsub#owner") {
            currentPayloadParser = std::make_shared<PubSubOwnerSubscriptionsParser>(parsers);
        }
        if (element == "default" && ns == "http://jabber.org/protocol/pubsub#owner") {
            currentPayloadParser = std::make_shared<PubSubOwnerDefaultParser>(parsers);
        }
        if (element == "purge" && ns == "http://jabber.org/protocol/pubsub#owner") {
            currentPayloadParser = std::make_shared<PubSubOwnerPurgeParser>(parsers);
        }
        if (element == "affiliations" && ns == "http://jabber.org/protocol/pubsub#owner") {
            currentPayloadParser = std::make_shared<PubSubOwnerAffiliationsParser>(parsers);
        }
        if (element == "delete" && ns == "http://jabber.org/protocol/pubsub#owner") {
            currentPayloadParser = std::make_shared<PubSubOwnerDeleteParser>(parsers);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubOwnerPubSubParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (currentPayloadParser) {
                getPayloadInternal()->setPayload(std::dynamic_pointer_cast<PubSubOwnerPayload>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubOwnerPubSubParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
