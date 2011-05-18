/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/AuthRequestParser.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

AuthRequestParser::AuthRequestParser() : GenericElementParser<AuthRequest>(), depth_(0) {
}

void AuthRequestParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attribute) {
	if (depth_ == 0) {
		getElementGeneric()->setMechanism(attribute.getAttribute("mechanism"));
	}
	++depth_;
}

void AuthRequestParser::handleEndElement(const std::string&, const std::string&) {
	--depth_;
	if (depth_ == 0) {
		getElementGeneric()->setMessage(createSafeByteArray(Base64::decode(text_)));
	}
}

void AuthRequestParser::handleCharacterData(const std::string& text) {
	text_ += text;
}

}
