/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <boost/filesystem/path.hpp>
#include <string>

namespace Swift {
	/**
	 * Creates a path for the given UTF-8 encoded string.
	 * This works independently of global locale settings.
	 */
	SWIFTEN_API boost::filesystem::path stringToPath(const std::string&);

	/**
	 * Returns the UTF-8 representation of the given path
	 * This works independently of global locale settings.
	 */
	SWIFTEN_API std::string pathToString(const boost::filesystem::path&);
}


