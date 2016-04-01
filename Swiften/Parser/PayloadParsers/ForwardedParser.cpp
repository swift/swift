/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/ForwardedParser.h>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/IQParser.h>
#include <Swiften/Parser/MessageParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/DelayParser.h>
#include <Swiften/Parser/PresenceParser.h>

using namespace Swift;

ForwardedParser::ForwardedParser(PayloadParserFactoryCollection* factories) : factories_(factories), level_(TopLevel) {
}

void ForwardedParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level_ == PayloadLevel) {
        if (element == "iq" && ns == "jabber:client") { /* begin parsing a nested stanza? */
            childParser_ = std::dynamic_pointer_cast<StanzaParser>(std::make_shared<IQParser>(factories_));
        } else if (element == "message" && ns == "jabber:client") {
            childParser_ = std::dynamic_pointer_cast<StanzaParser>(std::make_shared<MessageParser>(factories_));
        } else if (element == "presence" && ns == "jabber:client") {
            childParser_ = std::dynamic_pointer_cast<StanzaParser>(std::make_shared<PresenceParser>(factories_));
        } else if (element == "delay" && ns == "urn:xmpp:delay") { /* nested delay payload */
            delayParser_ = std::make_shared<DelayParser>();
        }
    }
    if (childParser_) { /* parsing a nested stanza? */
        childParser_->handleStartElement(element, ns, attributes);
    }
    if (delayParser_) { /* parsing a nested delay payload? */
        delayParser_->handleStartElement(element, ns, attributes);
    }
    ++level_;
}

void ForwardedParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level_;
    if (childParser_ && level_ >= PayloadLevel) {
        childParser_->handleEndElement(element, ns);
    }
    if (childParser_ && level_ == PayloadLevel) {
        /* done parsing nested stanza */
        getPayloadInternal()->setStanza(childParser_->getStanza());
        childParser_.reset();
    }
    if (delayParser_ && level_ >= PayloadLevel) {
        delayParser_->handleEndElement(element, ns);
    }
    if (delayParser_ && level_ == PayloadLevel) {
        /* done parsing nested delay payload */
        getPayloadInternal()->setDelay(std::dynamic_pointer_cast<Delay>(delayParser_->getPayload()));
        delayParser_.reset();
    }
}

void ForwardedParser::handleCharacterData(const std::string& data) {
    if (childParser_) {
        childParser_->handleCharacterData(data);
    }
    if (delayParser_) {
        delayParser_->handleCharacterData(data);
    }
}
