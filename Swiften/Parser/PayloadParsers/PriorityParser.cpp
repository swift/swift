#include "Swiften/Parser/PayloadParsers/PriorityParser.h"

#include <boost/lexical_cast.hpp>

namespace Swift {

PriorityParser::PriorityParser() : level_(0) {
}

void PriorityParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++level_;
}

void PriorityParser::handleEndElement(const String&, const String&) {
	--level_;
	if (level_ == 0) {
		getPayloadInternal()->setPriority(boost::lexical_cast<int>(text_));
	}
}

void PriorityParser::handleCharacterData(const String& data) {
	text_ += data;
}

}
