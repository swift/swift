/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/AuthChallengeParser.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

AuthChallengeParser::AuthChallengeParser() : GenericElementParser<AuthChallenge>(), depth(0) {
}

void AuthChallengeParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
	++depth;
}

void AuthChallengeParser::handleEndElement(const std::string&, const std::string&) {
	--depth;
	if (depth == 0) {
		getElementGeneric()->setValue(Base64::decode(text));
	}
}

void AuthChallengeParser::handleCharacterData(const std::string& text) {
	this->text += text;
}

}
