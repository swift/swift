/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/DelayParserFactory.h>

#include <boost/date_time/time_facet.hpp>

namespace Swift {

DelayParserFactory::DelayParserFactory() {
	boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet("%Y-%m-%d %H:%M:%S%F%Q");
	locale = std::locale(std::locale::classic(), facet);
}

}
