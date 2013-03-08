/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <boost/date_time/posix_time/ptime.hpp>

namespace Swift {
	/**
	 * Converts a date formatted according to XEP-0082 into a ptime
	 * object (in UTC).
	 */
	SWIFTEN_API boost::posix_time::ptime stringToDateTime(const std::string& string);

	/**
	 * Converts a UTC ptime object to a XEP-0082 formatted string.
	 */
	SWIFTEN_API std::string dateTimeToString(const boost::posix_time::ptime& time);

	/**
	 * Converts a UTC ptime object to a localized human readable string.
	 */
	SWIFTEN_API std::string dateTimeToLocalString(const boost::posix_time::ptime& time);
}
