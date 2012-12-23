/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Application/WindowsApplicationPathProvider.h>

#include <windows.h>
#include <cassert>

namespace Swift {

WindowsApplicationPathProvider::WindowsApplicationPathProvider(const std::string& name) : ApplicationPathProvider(name) {
	resourceDirs.push_back(getExecutableDir());
	resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
}

boost::filesystem::path WindowsApplicationPathProvider::getDataDir() const {
	char* appDirRaw = getenv("APPDATA");
	assert(appDirRaw);
	boost::filesystem::path result(boost::filesystem::path(appDirRaw) / getApplicationName());
	boost::filesystem::create_directory(result);
	return result;
}

boost::filesystem::path WindowsApplicationPathProvider::getHomeDir() const {
	//FIXME: This should be My Documents 
	char* homeDirRaw = getenv("USERPROFILE");
	assert(homeDirRaw);
	return boost::filesystem::path(homeDirRaw);
}


}
