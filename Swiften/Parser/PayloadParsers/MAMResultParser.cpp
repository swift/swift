/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/MAMResultParser.h>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/PayloadParsers/ForwardedParser.h>

using namespace Swift;

MAMResultParser::MAMResultParser(PayloadParserFactoryCollection* factories) : factories_(factories), level_(TopLevel) {
}

void MAMResultParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level_ == TopLevel) {
        boost::optional<std::string> attributeValue;
        if ((attributeValue = attributes.getAttributeValue("id"))) {
            getPayloadInternal()->setID(*attributeValue);
        }
        if ((attributeValue = attributes.getAttributeValue("queryid"))) {
            getPayloadInternal()->setQueryID(*attributeValue);
        }
    } else if (level_ == PayloadLevel) {
        if (element == "forwarded" && ns == "urn:xmpp:forward:0") {
            payloadParser_ = std::make_shared<ForwardedParser>(factories_);
        }
    }

    if (payloadParser_) {
        /* parsing a nested payload */
        payloadParser_->handleStartElement(element, ns, attributes);
    }

    ++level_;
}

void MAMResultParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level_;
    if (payloadParser_ && level_ >= PayloadLevel) {
        payloadParser_->handleEndElement(element, ns);
    }
    if (payloadParser_ && level_ == PayloadLevel) {
        /* done parsing nested stanza */
        getPayloadInternal()->setPayload(std::dynamic_pointer_cast<Forwarded>(payloadParser_->getPayload()));
        payloadParser_.reset();
    }
}

void MAMResultParser::handleCharacterData(const std::string& data) {
    if (payloadParser_) {
        payloadParser_->handleCharacterData(data);
    }
}
