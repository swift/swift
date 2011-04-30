/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/NicknameParser.h>

namespace Swift {

NicknameParser::NicknameParser() : level(0) {
}

void NicknameParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
	++level;
}

void NicknameParser::handleEndElement(const std::string&, const std::string&) {
	--level;
	if (level == 0) {
		getPayloadInternal()->setNickname(text);
	}
}

void NicknameParser::handleCharacterData(const std::string& data) {
	text += data;
}

}
