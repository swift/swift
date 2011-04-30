/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/SubjectParser.h>

namespace Swift {

SubjectParser::SubjectParser() : level_(0) {
}

void SubjectParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
	++level_;
}

void SubjectParser::handleEndElement(const std::string&, const std::string&) {
	--level_;
	if (level_ == 0) {
		getPayloadInternal()->setText(text_);
	}
}

void SubjectParser::handleCharacterData(const std::string& data) {
	text_ += data;
}

}
