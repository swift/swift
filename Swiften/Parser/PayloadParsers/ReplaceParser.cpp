/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/ReplaceParser.h>

namespace Swift {

	ReplaceParser::ReplaceParser() : level_(0) {
	}

	void ReplaceParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
		if (level_ == 0) {
			std::string id = attributes.getAttribute("id");
			getPayloadInternal()->setID(id);
		}
		level_++;
	}

	void ReplaceParser::handleEndElement(const std::string&, const std::string&) {
		--level_;
	}

	void ReplaceParser::handleCharacterData(const std::string&) {
	}

}
