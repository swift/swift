/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Parser/PayloadParsers/PubSubRetractParser.h>

#include <boost/optional.hpp>


#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PubSubItemParser.h>

using namespace Swift;

PubSubRetractParser::PubSubRetractParser(PayloadParserFactoryCollection* parsers) : parsers(parsers), level(0) {
}

PubSubRetractParser::~PubSubRetractParser() {
}

void PubSubRetractParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level == 0) {
		if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
			getPayloadInternal()->setNode(*attributeValue);
		}
		if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("notify")) {
			getPayloadInternal()->setNotify(*attributeValue == "true" ? true : false);
		}
	}

	if (level == 1) {
		if (element == "item" && ns == "http://jabber.org/protocol/pubsub") {
			currentPayloadParser = boost::make_shared<PubSubItemParser>(parsers);
		}
	}

	if (level >= 1 && currentPayloadParser) {
		currentPayloadParser->handleStartElement(element, ns, attributes);
	}
	++level;
}

void PubSubRetractParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level;
	if (currentPayloadParser) {
		if (level >= 1) {
			currentPayloadParser->handleEndElement(element, ns);
		}

		if (level == 1) {
			if (element == "item" && ns == "http://jabber.org/protocol/pubsub") {
				getPayloadInternal()->addItem(boost::dynamic_pointer_cast<PubSubItem>(currentPayloadParser->getPayload()));
			}
			currentPayloadParser.reset();
		}
	}
}

void PubSubRetractParser::handleCharacterData(const std::string& data) {
	if (level > 1 && currentPayloadParser) {
		currentPayloadParser->handleCharacterData(data);
	}
}
