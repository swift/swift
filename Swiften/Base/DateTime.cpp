/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/DateTime.h>

#include <locale>
#include <boost/date_time/time_facet.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/String.h>

namespace Swift {

boost::posix_time::ptime stringToDateTime(const std::string& string) {
	static std::locale locale(std::locale::classic(), new boost::local_time::local_time_input_facet("%Y-%m-%d %H:%M:%S%F%ZP"));
	std::istringstream stream(string); 
	stream.imbue(locale);
	boost::local_time::local_date_time result(boost::date_time::not_a_date_time);
	stream >> result;
	return result.utc_time();
}

std::string dateTimeToString(const boost::posix_time::ptime& time) {
	std::string stampString = std::string(boost::posix_time::to_iso_extended_string(time));
	String::replaceAll(stampString, ',', ".");
	stampString += "Z";
	return stampString;
}

}
