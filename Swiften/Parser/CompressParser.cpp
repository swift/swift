/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/CompressParser.h"

namespace Swift {

CompressParser::CompressParser() : GenericElementParser<CompressRequest>(), currentDepth_(0), inMethod_(false) {
}

void CompressParser::handleStartElement(const String& element, const String&, const AttributeMap&) {
	if (currentDepth_ == 1 && element == "method") {
		inMethod_ = true;
		currentText_ = "";
	}
	++currentDepth_;
}

void CompressParser::handleEndElement(const String&, const String&) {
	--currentDepth_;
	if (currentDepth_ == 1 && inMethod_) {
		getElementGeneric()->setMethod(currentText_);
		inMethod_ = false;
	}
}

void CompressParser::handleCharacterData(const String& data) {
	currentText_ += data;
}

}
