/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <SwifTools/Application/ApplicationPathProvider.h>

namespace Swift {
	class WindowsApplicationPathProvider : public ApplicationPathProvider {
		public:
			WindowsApplicationPathProvider(const std::string& name);

			boost::filesystem::path getDataDir() const;
			boost::filesystem::path getHomeDir() const;

			virtual std::vector<boost::filesystem::path> getResourceDirs() const {
				return resourceDirs;
			}

		private:
			std::vector<boost::filesystem::path> resourceDirs;
	};
}
