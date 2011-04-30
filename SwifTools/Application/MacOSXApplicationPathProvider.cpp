/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Application/MacOSXApplicationPathProvider.h>

#include <iostream>
#include <mach-o/dyld.h>

namespace Swift {

MacOSXApplicationPathProvider::MacOSXApplicationPathProvider(const std::string& name) : ApplicationPathProvider(name) {
	resourceDirs.push_back(getExecutableDir() / "../Resources");
	resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
}

boost::filesystem::path MacOSXApplicationPathProvider::getDataDir() const {
	boost::filesystem::path result(getHomeDir() / "Library/Application Support" / getApplicationName());
	try {
		boost::filesystem::create_directory(result);
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return result;
}

boost::filesystem::path MacOSXApplicationPathProvider::getHomeDir() const {
	return boost::filesystem::path(getenv("HOME"));
}

}
