/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/NicknameParser.h"

namespace Swift {

NicknameParser::NicknameParser() : level(0) {
}

void NicknameParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++level;
}

void NicknameParser::handleEndElement(const String&, const String&) {
	--level;
	if (level == 0) {
		getPayloadInternal()->setNickname(text);
	}
}

void NicknameParser::handleCharacterData(const String& data) {
	text += data;
}

}
