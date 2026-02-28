/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubItemsParser.h>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubItemParser.h>

using namespace Swift;

PubSubItemsParser::PubSubItemsParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubItemsParser::~PubSubItemsParser() {
}

void PubSubItemsParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("max_items")) {
            try {
                getPayloadInternal()->setMaximumItems(boost::lexical_cast<unsigned int>(*attributeValue));
            }
            catch (boost::bad_lexical_cast&) {
            }
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("subid")) {
            getPayloadInternal()->setSubscriptionID(*attributeValue);
        }
    }

    if (level == 1) {
        if (element == "item" && ns == "http://jabber.org/protocol/pubsub") {
            currentPayloadParser = std::make_shared<PubSubItemParser>(parsers);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubItemsParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (element == "item" && ns == "http://jabber.org/protocol/pubsub") {
                getPayloadInternal()->addItem(std::dynamic_pointer_cast<PubSubItem>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubItemsParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
