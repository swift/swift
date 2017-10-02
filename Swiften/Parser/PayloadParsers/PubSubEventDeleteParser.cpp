/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Parser/PayloadParsers/PubSubEventDeleteParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/PubSubEventRedirectParser.h>

using namespace Swift;

PubSubEventDeleteParser::PubSubEventDeleteParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubEventDeleteParser::~PubSubEventDeleteParser() {
}

void PubSubEventDeleteParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
    }

    if (level == 1) {
        if (element == "redirect" && ns == "http://jabber.org/protocol/pubsub#event") {
            currentPayloadParser = std::make_shared<PubSubEventRedirectParser>(parsers);
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubEventDeleteParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (element == "redirect" && ns == "http://jabber.org/protocol/pubsub#event") {
                getPayloadInternal()->setRedirects(std::dynamic_pointer_cast<PubSubEventRedirect>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubEventDeleteParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
