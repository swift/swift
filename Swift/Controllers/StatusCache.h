/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <list>
#include <iostream>

#include <boost/filesystem/path.hpp>

#include <Swiften/Elements/StatusShow.h>

namespace Swift {
	class ApplicationPathProvider;
	class StatusCache {
		public:
			typedef std::pair<std::string, StatusShow::Type> PreviousStatus;
		public:
			StatusCache(ApplicationPathProvider* paths);
			~StatusCache();

			std::vector<PreviousStatus> getMatches(const std::string& substring, size_t maxCount) const;
			void addRecent(const std::string& text, StatusShow::Type type);
		private:
			void saveRecents();
			void loadRecents();
		private:
			boost::filesystem::path path_;
			std::list<PreviousStatus> previousStatuses_;
			ApplicationPathProvider* paths_;
	};
}


