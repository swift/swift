/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/LastParser.h>

#include <boost/lexical_cast.hpp>

namespace Swift {

LastParser::LastParser() : level_(0) {
}

void LastParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
	if (level_ == 0) {
		int seconds = 0;
		try {
			seconds = boost::lexical_cast<int>(attributes.getAttribute("seconds"));
		}
		catch (boost::bad_lexical_cast&) {
		}
		getPayloadInternal()->setSeconds(seconds);
	}
	++level_;

}

void LastParser::handleEndElement(const std::string&, const std::string&) {
}

void LastParser::handleCharacterData(const std::string&) {
}

}
