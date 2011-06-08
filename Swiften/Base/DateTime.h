/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/date_time/posix_time/ptime.hpp>

namespace Swift {
	/**
	 * Converts a date formatted according to XEP-0082 into a time
	 * object.
	 */
	boost::posix_time::ptime stringToDateTime(const std::string& string);
}
