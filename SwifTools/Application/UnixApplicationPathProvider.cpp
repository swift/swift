/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SwifTools/Application/UnixApplicationPathProvider.h"

#include <boost/filesystem/convenience.hpp>

namespace Swift {

UnixApplicationPathProvider::UnixApplicationPathProvider(const String& name) : ApplicationPathProvider(name) {
	resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
	char* xdgDataDirs = getenv("XDG_DATA_DIRS");
	if (xdgDataDirs) {
		std::vector<String> dataDirs = String(xdgDataDirs).split(':');
		if (!dataDirs.empty()) {
			foreach(const String& dir, dataDirs) {
				resourceDirs.push_back(boost::filesystem::path(dir.getUTF8String()) / "swift");
			}
			return;
		}
	}
	resourceDirs.push_back("/usr/local/share/swift");
	resourceDirs.push_back("/usr/share/swift");
}

boost::filesystem::path UnixApplicationPathProvider::getHomeDir() const {
	return boost::filesystem::path(getenv("HOME"));
}

boost::filesystem::path UnixApplicationPathProvider::getDataDir() const {
	char* xdgDataHome = getenv("XDG_DATA_HOME");
	String dataDir;
	if (xdgDataHome) {
		dataDir = String(xdgDataHome);
	}

	boost::filesystem::path dataPath = (dataDir.isEmpty() ? 
			getHomeDir() / ".local" / "share" 
			: boost::filesystem::path(dataDir.getUTF8String())) / getApplicationName().getLowerCase().getUTF8String();

	try {
		boost::filesystem::create_directories(dataPath);
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return dataPath;
}

boost::filesystem::path UnixApplicationPathProvider::getExecutableDir() const {
	ByteArray path;
	path.resize(4096);
	size_t size = readlink("/proc/self/exe", path.getData(), path.getSize());
	if (size > 0) {
		path.resize(size);
		return boost::filesystem::path(path.toString().getUTF8Data()).parent_path();
	}
	else {
		return boost::filesystem::path();
	}
}

}
