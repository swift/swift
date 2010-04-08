/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/DiscoInfoParser.h"

namespace Swift {

DiscoInfoParser::DiscoInfoParser() : level_(TopLevel) {
}

void DiscoInfoParser::handleStartElement(const String& element, const String&, const AttributeMap& attributes) {
	if (level_ == PayloadLevel) {
		if (element == "identity") {
			getPayloadInternal()->addIdentity(DiscoInfo::Identity(attributes.getAttribute("name"), attributes.getAttribute("category"), attributes.getAttribute("type"), attributes.getAttribute("lang")));
		}
		else if (element == "feature") {
			getPayloadInternal()->addFeature(attributes.getAttribute("var"));
		}
	}
	++level_;
}

void DiscoInfoParser::handleEndElement(const String&, const String&) {
	--level_;
}

void DiscoInfoParser::handleCharacterData(const String&) {
}

}
