/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
