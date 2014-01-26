/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Parser/PayloadParsers/PubSubConfigureParser.h>

#include <boost/optional.hpp>


#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

using namespace Swift;

PubSubConfigureParser::PubSubConfigureParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubConfigureParser::~PubSubConfigureParser() {
}

void PubSubConfigureParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	

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

void PubSubConfigureParser::handleEndElement(const std::string& element, const std::string& ns) {
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

void PubSubConfigureParser::handleCharacterData(const std::string& data) {
	if (level > 1 && currentPayloadParser) {
		currentPayloadParser->handleCharacterData(data);
	}
}
