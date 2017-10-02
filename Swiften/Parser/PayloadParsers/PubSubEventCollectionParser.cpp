/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubEventCollectionParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventAssociateParser.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventDisassociateParser.h>

using namespace Swift;

PubSubEventCollectionParser::PubSubEventCollectionParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubEventCollectionParser::~PubSubEventCollectionParser() {
}

void PubSubEventCollectionParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
    }

    if (level == 1) {
        if (element == "disassociate" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventDisassociateParser>(parsers);
        }
        if (element == "associate" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventAssociateParser>(parsers);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubEventCollectionParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (element == "disassociate" && ns == "http://jabber.org/protocol/pubsub#event") {
                getPayloadInternal()->setDisassociate(std::dynamic_pointer_cast<PubSubEventDisassociate>(currentPayloadParser->getPayload()));
            }
            if (element == "associate" && ns == "http://jabber.org/protocol/pubsub#event") {
                getPayloadInternal()->setAssociate(std::dynamic_pointer_cast<PubSubEventAssociate>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubEventCollectionParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
