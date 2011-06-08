/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/DateTime.h>

#include <locale>

#include <boost/date_time/time_facet.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Swift {

boost::posix_time::ptime stringToDateTime(const std::string& string) {
	static std::locale locale(std::locale::classic(), new boost::posix_time::time_input_facet("%Y-%m-%d %H:%M:%S%F%Q"));
	std::istringstream stream(string); 
	stream.imbue(locale);
	boost::posix_time::ptime result(boost::posix_time::not_a_date_time);
	stream >> result;
	return result;
}

}
