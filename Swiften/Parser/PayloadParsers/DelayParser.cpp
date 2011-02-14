/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/DelayParser.h"

#include <locale>

#include <boost/date_time/time_facet.hpp>

namespace Swift {

DelayParser::DelayParser(const std::locale& locale) : locale(locale), level_(0) {
}

boost::posix_time::ptime DelayParser::dateFromString(const std::string& string) {
	std::istringstream stream(string); 
	stream.imbue(locale);
	boost::posix_time::ptime result(boost::posix_time::not_a_date_time);
	stream >> result;
	return result;
}

void DelayParser::handleStartElement(const std::string& /*element*/, const std::string& /*ns*/, const AttributeMap& attributes) {
	if (level_ == 0) {
		boost::posix_time::ptime stamp = dateFromString(attributes.getAttribute("stamp"));
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
