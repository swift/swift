/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Parser/PayloadParsers/PubSubDefaultParser.h>

#include <boost/optional.hpp>


#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/EnumParser.h>

using namespace Swift;

PubSubDefaultParser::PubSubDefaultParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubDefaultParser::~PubSubDefaultParser() {
}

void PubSubDefaultParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level == 0) {
		if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
			getPayloadInternal()->setNode(*attributeValue);
		}
		if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("type")) {
			if (boost::optional<PubSubDefault::Type> value = EnumParser<PubSubDefault::Type>()(PubSubDefault::None, "none")(PubSubDefault::Collection, "collection")(PubSubDefault::Leaf, "leaf").parse(*attributeValue)) {
				getPayloadInternal()->setType(*value);
			}
		}
	}

	

	if (level >= 1 && currentPayloadParser) {
		currentPayloadParser->handleStartElement(element, ns, attributes);
	}
	++level;
}

void PubSubDefaultParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level;
	if (currentPayloadParser) {
		if (level >= 1) {
			currentPayloadParser->handleEndElement(element, ns);
		}

		if (level == 1) {
			
			currentPayloadParser.reset();
		}
	}
}

void PubSubDefaultParser::handleCharacterData(const std::string& data) {
	if (level > 1 && currentPayloadParser) {
		currentPayloadParser->handleCharacterData(data);
	}
}
