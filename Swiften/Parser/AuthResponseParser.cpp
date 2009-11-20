#include "Swiften/Parser/AuthResponseParser.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

AuthResponseParser::AuthResponseParser() : GenericElementParser<AuthResponse>(), depth(0) {
}

void AuthResponseParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++depth;
}

void AuthResponseParser::handleEndElement(const String&, const String&) {
	--depth;
	if (depth == 0) {
		getElementGeneric()->setValue(Base64::decode(text));
	}
}

void AuthResponseParser::handleCharacterData(const String& text) {
	this->text += text;
}

}
