/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <SwifTools/Application/ApplicationPathProvider.h>

namespace Swift {
	class UnixApplicationPathProvider : public ApplicationPathProvider {
		public:
			UnixApplicationPathProvider(const std::string& name);

			virtual boost::filesystem::path getHomeDir() const;
			boost::filesystem::path getDataDir() const;

			virtual std::vector<boost::filesystem::path> getResourceDirs() const {
				return resourceDirs;
			}

		private:
			std::vector<boost::filesystem::path> resourceDirs;
	};
}

