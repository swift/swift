#include "Swiften/Parser/PayloadParsers/StatusParser.h"

namespace Swift {

StatusParser::StatusParser() : level_(0) {
}

void StatusParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++level_;
}

void StatusParser::handleEndElement(const String&, const String&) {
	--level_;
	if (level_ == 0) {
		getPayloadInternal()->setText(text_);
	}
}

void StatusParser::handleCharacterData(const String& data) {
	text_ += data;
}

}
