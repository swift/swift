/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/DiscoItemsParser.h>

namespace Swift {

DiscoItemsParser::DiscoItemsParser() : level_(TopLevel) {
}

void DiscoItemsParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (level_ == PayloadLevel) {
		if (element == "item") {
			getPayloadInternal()->addItem(DiscoItems::Item(attributes.getAttribute("name"), JID(attributes.getAttribute("jid")), attributes.getAttribute("node")));
		}
	}
	++level_;
}

void DiscoItemsParser::handleEndElement(const std::string&, const std::string&) {
	--level_;
}

void DiscoItemsParser::handleCharacterData(const std::string&) {
}

}
