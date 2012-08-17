/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace Swift {
	class CrashReporter {
		public:
			CrashReporter(const boost::filesystem::path& path);

		private:
			struct Private;
			boost::shared_ptr<Private> p;
	};
}
