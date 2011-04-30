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

			boost::filesystem::path getDataDir() const {
				char* appDirRaw = getenv("APPDATA");
				boost::filesystem::path result(boost::filesystem::path(appDirRaw) / getApplicationName());
				boost::filesystem::create_directory(result);
				return result;
			}

			boost::filesystem::path getHomeDir() const {
				//FIXME: This should be My Documents 
				
				char* homeDirRaw = getenv("USERPROFILE");
				return boost::filesystem::path(homeDirRaw);
			}

			virtual std::vector<boost::filesystem::path> getResourceDirs() const {
				return resourceDirs;
			}

		private:
			std::vector<boost::filesystem::path> resourceDirs;
	};
}
