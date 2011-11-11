/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

#include <Swiften/Parser/PayloadParsers/DeliveryReceiptParser.h>

#include <boost/optional.hpp>

namespace Swift {

DeliveryReceiptParser::DeliveryReceiptParser() : level_(0) {
}

void DeliveryReceiptParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributeMap) {
	if (level_ == 0) {
		if (element == "received") {
			if (attributeMap.getAttributeValue("id").is_initialized()) {
				getPayloadInternal()->setReceivedID(attributeMap.getAttributeValue("id").get());
			}
		}
	}
	++level_;
}

void DeliveryReceiptParser::handleEndElement(const std::string&, const std::string&) {
	--level_;
}

void DeliveryReceiptParser::handleCharacterData(const std::string&) {

}

}
