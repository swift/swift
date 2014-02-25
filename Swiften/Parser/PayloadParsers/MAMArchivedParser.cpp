/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParsers/MAMArchivedParser.h>

using namespace Swift;

MAMArchivedParser::MAMArchivedParser() : level_(TopLevel) {
}

void MAMArchivedParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
	if (level_ == TopLevel) {
		boost::optional<std::string> attributeValue;
		if ((attributeValue = attributes.getAttributeValue("by"))) {
			getPayloadInternal()->setBy(*attributeValue);
		}
		if ((attributeValue = attributes.getAttributeValue("id"))) {
			getPayloadInternal()->setID(*attributeValue);
		}
	}

	++level_;
}

void MAMArchivedParser::handleEndElement(const std::string&, const std::string&) {
	--level_;
}

void MAMArchivedParser::handleCharacterData(const std::string&) {
}
