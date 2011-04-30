/*
 * Copyright (c) 2010 Remko TronÃ§on
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/filesystem.hpp>
#include <iostream>

#include <SwifTools/Application/ApplicationPathProvider.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Paths.h>

namespace Swift {

ApplicationPathProvider::ApplicationPathProvider(const std::string& applicationName) : applicationName(applicationName) {
}

ApplicationPathProvider::~ApplicationPathProvider() {
}

boost::filesystem::path ApplicationPathProvider::getProfileDir(const std::string& profile) const {
	boost::filesystem::path result(getHomeDir() / profile);
	try {
		boost::filesystem::create_directory(result);
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return result;
}

boost::filesystem::path ApplicationPathProvider::getResourcePath(const std::string& resource) const {
	std::vector<boost::filesystem::path> resourcePaths = getResourceDirs();
	foreach(const boost::filesystem::path& resourcePath, resourcePaths) {
		boost::filesystem::path r(resourcePath / resource);
		if (boost::filesystem::exists(r)) {
			return r;
		}
	}
	return boost::filesystem::path();
}

boost::filesystem::path ApplicationPathProvider::getExecutableDir() const {
	return Paths::getExecutablePath();
}

}
