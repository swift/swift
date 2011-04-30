/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/ComponentHandshakeParser.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

ComponentHandshakeParser::ComponentHandshakeParser() : GenericElementParser<ComponentHandshake>(), depth(0) {
}

void ComponentHandshakeParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
	++depth;
}

void ComponentHandshakeParser::handleEndElement(const std::string&, const std::string&) {
	--depth;
	if (depth == 0) {
		getElementGeneric()->setData(text);
	}
}

void ComponentHandshakeParser::handleCharacterData(const std::string& text) {
	this->text += text;
}

}
