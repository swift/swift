#include "Swiften/Parser/AuthChallengeParser.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

AuthChallengeParser::AuthChallengeParser() : GenericElementParser<AuthChallenge>(), depth(0) {
}

void AuthChallengeParser::handleStartElement(const String&, const String&, const AttributeMap&) {
	++depth;
}

void AuthChallengeParser::handleEndElement(const String&, const String&) {
	--depth;
	if (depth == 0) {
		getElementGeneric()->setValue(Base64::decode(text));
	}
}

void AuthChallengeParser::handleCharacterData(const String& text) {
	this->text += text;
}

}
