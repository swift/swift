/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>
#include <vector>
#include <string>

namespace Swift {
	class ApplicationPathProvider {
		public:
			ApplicationPathProvider(const std::string& applicationName);
			virtual ~ApplicationPathProvider();

			virtual boost::filesystem::path getHomeDir() const = 0;
			virtual boost::filesystem::path getDataDir() const = 0;
			virtual boost::filesystem::path getExecutableDir() const;
			boost::filesystem::path getProfileDir(const std::string& profile) const;
			boost::filesystem::path getResourcePath(const std::string& resource) const;

		protected:
			virtual std::vector<boost::filesystem::path> getResourceDirs() const = 0;
			const std::string& getApplicationName() const {
				return applicationName;
			}

		private:
			std::string applicationName;
	};
}
