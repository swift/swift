/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/SoftwareVersionParser.h"

namespace Swift {

SoftwareVersionParser::SoftwareVersionParser() : level_(TopLevel) {
}

void SoftwareVersionParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++level_;
}

void SoftwareVersionParser::handleEndElement(const String& element, const String&) {
	--level_;
	if (level_ == PayloadLevel) {
		if (element == "name") {
			getPayloadInternal()->setName(currentText_);
		}
		else if (element == "version") {
			getPayloadInternal()->setVersion(currentText_);
		}
		else if (element == "os") {
			getPayloadInternal()->setOS(currentText_);
		}
		currentText_ = "";
	}
}

void SoftwareVersionParser::handleCharacterData(const String& data) {
	currentText_ += data;
}

}
