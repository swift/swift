/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubRetractParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubItemParser.h>

using namespace Swift;

PubSubRetractParser::PubSubRetractParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubRetractParser::~PubSubRetractParser() {
}

void PubSubRetractParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("notify")) {
            boost::optional<bool> notify;
            if (attributeValue.is_initialized()) {
                notify = (attributeValue.get() == "true" || attributeValue.get() == "1") ? true : false;
            }
            getPayloadInternal()->setNotify(notify);
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

void PubSubRetractParser::handleEndElement(const std::string& element, const std::string& ns) {
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

void PubSubRetractParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
