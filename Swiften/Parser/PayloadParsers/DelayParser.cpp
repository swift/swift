/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/DelayParser.h"

#include <locale>

#include <boost/date_time/time_facet.hpp>

namespace Swift {

DelayParser::DelayParser() : level_(0) {
}

boost::posix_time::ptime DelayParser::dateFromString(const String& string) {
	boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet("%Y-%m-%d %H:%M:%S%F%Q");
	boost::posix_time::ptime result(boost::posix_time::not_a_date_time);
	std::locale dateLocale(std::locale::classic(), facet);
	std::istringstream stream(string.getUTF8String()); 
	stream.imbue(dateLocale);
	stream >> result;
	return result;
}

void DelayParser::handleStartElement(const String& /*element*/, const String& /*ns*/, const AttributeMap& attributes) {
	if (level_ == 0) {
		boost::posix_time::ptime stamp = dateFromString(attributes.getAttribute("stamp"));
		getPayloadInternal()->setStamp(stamp);
		if (!attributes.getAttribute("from").isEmpty()) {
			String from = attributes.getAttribute("from");
			getPayloadInternal()->setFrom(JID(from));
		}
	}
	++level_;
}

void DelayParser::handleEndElement(const String&, const String&) {
	--level_;
}

void DelayParser::handleCharacterData(const String&) {

}

}
