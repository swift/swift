/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/BodyParser.h"

namespace Swift {

BodyParser::BodyParser() : level_(0) {
}

void BodyParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++level_;
}

void BodyParser::handleEndElement(const String&, const String&) {
	--level_;
	if (level_ == 0) {
		getPayloadInternal()->setText(text_);
	}
}

void BodyParser::handleCharacterData(const String& data) {
	text_ += data;
}

}
