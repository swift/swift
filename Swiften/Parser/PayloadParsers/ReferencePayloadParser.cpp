/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/ReferencePayloadParser.h>

#include <cassert>
#include <iostream>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>

namespace Swift {

ReferencePayloadParser::ReferencePayloadParser(PayloadParserFactoryCollection* factories) : factories_(factories) {
}

ReferencePayload::Type ReferencePayloadParser::getTypeFromString(const std::string& typeString) const {
    if (typeString == "data") {
        return ReferencePayload::Type::Data;
    }
    else if (typeString == "mention") {
        return ReferencePayload::Type::Mention;
    }
    else if (typeString == "pubsub") {
        return ReferencePayload::Type::PubSub;
    }
    else {
        return ReferencePayload::Type::Unknown;
    }
}

void ReferencePayloadParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level_ == topLevel_) {
        if (element == "reference") {
            getPayloadInternal()->setType(getTypeFromString(attributes.getAttribute("type")));
            getPayloadInternal()->setUri(attributes.getAttributeValue("uri"));
            getPayloadInternal()->setBegin(attributes.getAttributeValue("begin"));
            getPayloadInternal()->setEnd(attributes.getAttributeValue("end"));
            getPayloadInternal()->setAnchor(attributes.getAttributeValue("anchor"));
        }
    }
    else if (level_ == payloadLevel_)   {
        PayloadParserFactory* payloadParserFactory = factories_->getPayloadParserFactory(element, ns, attributes);
        if (payloadParserFactory) {
            currentPayloadParser_.reset(payloadParserFactory->createPayloadParser());
        }
    }

    if (level_ >= payloadLevel_ && currentPayloadParser_) {
        currentPayloadParser_->handleStartElement(element, ns, attributes);
    }

    ++level_;
}

void ReferencePayloadParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level_;
    if (currentPayloadParser_) {
        if (level_ >= payloadLevel_) {
            currentPayloadParser_->handleEndElement(element, ns);
        }

        if (level_ == payloadLevel_) {
            getPayloadInternal()->addPayload(currentPayloadParser_->getPayload());
            currentPayloadParser_.reset();
        }
    }
}

void ReferencePayloadParser::handleCharacterData(const std::string& data) {
    if (level_ > payloadLevel_ && currentPayloadParser_) {
        currentPayloadParser_->handleCharacterData(data);
    }
}

}
