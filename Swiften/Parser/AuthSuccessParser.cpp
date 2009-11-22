#include "Swiften/Parser/AuthSuccessParser.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

AuthSuccessParser::AuthSuccessParser() : GenericElementParser<AuthSuccess>(), depth(0) {
}

void AuthSuccessParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++depth;
}

void AuthSuccessParser::handleEndElement(const String&, const String&) {
	--depth;
	if (depth == 0) {
		getElementGeneric()->setValue(Base64::decode(text));
	}
}

void AuthSuccessParser::handleCharacterData(const String& text) {
	this->text += text;
}

}
