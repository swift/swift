/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/DelayParser.h>

#include <Swiften/Base/DateTime.h>

namespace Swift {

DelayParser::DelayParser() : level_(0) {
}

void DelayParser::handleStartElement(const std::string& /*element*/, const std::string& /*ns*/, const AttributeMap& attributes) {
	if (level_ == 0) {
		boost::posix_time::ptime stamp = stringToDateTime(attributes.getAttribute("stamp"));
		getPayloadInternal()->setStamp(stamp);
		if (!attributes.getAttribute("from").empty()) {
			std::string from = attributes.getAttribute("from");
			getPayloadInternal()->setFrom(JID(from));
		}
	}
	++level_;
}

void DelayParser::handleEndElement(const std::string&, const std::string&) {
	--level_;
}

void DelayParser::handleCharacterData(const std::string&) {

}

}
