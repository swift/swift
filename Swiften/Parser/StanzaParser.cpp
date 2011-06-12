/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/StanzaParser.h>

#include <iostream>
#include <boost/optional.hpp>
#include <cassert>

#include <Swiften/Parser/PayloadParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/UnknownPayloadParser.h>

namespace Swift {

StanzaParser::StanzaParser(PayloadParserFactoryCollection* factories) : 
		currentDepth_(0), factories_(factories) {
}

StanzaParser::~StanzaParser() {
}

void StanzaParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (inStanza()) {
		if (!inPayload()) {
			assert(!currentPayloadParser_);
			PayloadParserFactory* payloadParserFactory = factories_->getPayloadParserFactory(element, ns, attributes);
			if (payloadParserFactory) {
				currentPayloadParser_.reset(payloadParserFactory->createPayloadParser());
			}
			else {
				currentPayloadParser_.reset(new UnknownPayloadParser());
			}
		}
		assert(currentPayloadParser_);
		currentPayloadParser_->handleStartElement(element, ns, attributes);
	}
	else {
		boost::optional<std::string> from = attributes.getAttributeValue("from");
		if (from) {
			getStanza()->setFrom(JID(*from));
		}
		boost::optional<std::string> to = attributes.getAttributeValue("to");
		if (to) {
			getStanza()->setTo(JID(*to));
		}
		boost::optional<std::string> id = attributes.getAttributeValue("id");
		if (id) {
			getStanza()->setID(*id);
		}
		handleStanzaAttributes(attributes);
	}
	++currentDepth_;
}

void StanzaParser::handleEndElement(const std::string& element, const std::string& ns) {
	assert(inStanza());
	if (inPayload()) {
		assert(currentPayloadParser_);
		currentPayloadParser_->handleEndElement(element, ns);
		--currentDepth_;
		if (!inPayload()) {
			boost::shared_ptr<Payload> payload(currentPayloadParser_->getPayload());
			if (payload) {
				getStanza()->addPayload(payload);
			}
			currentPayloadParser_.reset();
		}
	}
	else {
		--currentDepth_;
	}
}

void StanzaParser::handleCharacterData(const std::string& data) {
	if (currentPayloadParser_) {
		currentPayloadParser_->handleCharacterData(data);
	}
}

}
