/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Parser/PayloadParsers/PubSubOwnerConfigureParser.h>

#include <boost/optional.hpp>


#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

using namespace Swift;

PubSubOwnerConfigureParser::PubSubOwnerConfigureParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubOwnerConfigureParser::~PubSubOwnerConfigureParser() {
}

void PubSubOwnerConfigureParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
    }

    if (level == 1) {
        if (element == "x" && ns == "jabber:x:data") {
            currentPayloadParser = boost::make_shared<FormParser>();
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubOwnerConfigureParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            if (element == "x" && ns == "jabber:x:data") {
                getPayloadInternal()->setData(boost::dynamic_pointer_cast<Form>(currentPayloadParser->getPayload()));
            }
            currentPayloadParser.reset();
        }
    }
}

void PubSubOwnerConfigureParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
