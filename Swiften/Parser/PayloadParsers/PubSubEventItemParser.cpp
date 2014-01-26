/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Parser/PayloadParsers/PubSubEventItemParser.h>

#include <boost/optional.hpp>


#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>


using namespace Swift;

PubSubEventItemParser::PubSubEventItemParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubEventItemParser::~PubSubEventItemParser() {
}

void PubSubEventItemParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level == 0) {
		if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
			getPayloadInternal()->setNode(*attributeValue);
		}
		if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("publisher")) {
			getPayloadInternal()->setPublisher(*attributeValue);
		}
		if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("id")) {
			getPayloadInternal()->setID(*attributeValue);
		}
	}

	if (level == 1) {
		if (PayloadParserFactory* factory = parsers->getPayloadParserFactory(element, ns, attributes)) {
			currentPayloadParser.reset(factory->createPayloadParser());
		}
	}

	if (level >= 1 && currentPayloadParser) {
		currentPayloadParser->handleStartElement(element, ns, attributes);
	}
	++level;
}

void PubSubEventItemParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level;
	if (currentPayloadParser) {
		if (level >= 1) {
			currentPayloadParser->handleEndElement(element, ns);
		}

		if (level == 1) {
			getPayloadInternal()->addData(currentPayloadParser->getPayload());
			currentPayloadParser.reset();
		}
	}
}

void PubSubEventItemParser::handleCharacterData(const std::string& data) {
	if (level > 1 && currentPayloadParser) {
		currentPayloadParser->handleCharacterData(data);
	}
}
