/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/AuthResponseParser.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

AuthResponseParser::AuthResponseParser() : GenericElementParser<AuthResponse>(), depth(0) {
}

void AuthResponseParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
	++depth;
}

void AuthResponseParser::handleEndElement(const std::string&, const std::string&) {
	--depth;
	if (depth == 0) {
		getElementGeneric()->setValue(createSafeByteArray(Base64::decode(text)));
	}
}

void AuthResponseParser::handleCharacterData(const std::string& text) {
	this->text += text;
}

}
