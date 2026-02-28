/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PrivateStorageParser.h>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>

namespace Swift {

PrivateStorageParser::PrivateStorageParser(PayloadParserFactoryCollection* factories) : factories(factories), level(0) {
}

void PrivateStorageParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 1) {
        PayloadParserFactory* payloadParserFactory = factories->getPayloadParserFactory(element, ns, attributes);
        if (payloadParserFactory) {
            currentPayloadParser.reset(payloadParserFactory->createPayloadParser());
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PrivateStorageParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {
            getPayloadInternal()->setPayload(currentPayloadParser->getPayload());
        }
    }
}

void PrivateStorageParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}

}
