/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Application/WindowsApplicationPathProvider.h>

#include <windows.h>
#include <cassert>
#include <Swiften/Base/String.h>

namespace Swift {

WindowsApplicationPathProvider::WindowsApplicationPathProvider(const std::string& name) : ApplicationPathProvider(name) {
	resourceDirs.push_back(getExecutableDir());
	resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
}

boost::filesystem::path WindowsApplicationPathProvider::getDataDir() const {
	wchar_t* appDirRaw = _wgetenv(L"APPDATA");
	assert(appDirRaw);
	boost::filesystem::path result(
			boost::filesystem::path(appDirRaw) / getApplicationName());
	boost::filesystem::create_directory(result);
	return result;
}

boost::filesystem::path WindowsApplicationPathProvider::getHomeDir() const {
	//FIXME: This should be My Documents 
	wchar_t* homeDirRaw = _wgetenv(L"USERPROFILE");
	assert(homeDirRaw);
	return boost::filesystem::path(homeDirRaw);
}


}
