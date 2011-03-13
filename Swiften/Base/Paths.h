/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

namespace Swift {
	class Paths {
		public:
			static boost::filesystem::path getExecutablePath();
	};
}
