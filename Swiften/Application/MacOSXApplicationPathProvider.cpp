/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Application/MacOSXApplicationPathProvider.h"

#include <iostream>
#include <mach-o/dyld.h>

#include "Swiften/Base/ByteArray.h"

namespace Swift {

MacOSXApplicationPathProvider::MacOSXApplicationPathProvider(const String& name) : ApplicationPathProvider(name) {
}

boost::filesystem::path MacOSXApplicationPathProvider::getSettingsDir() const {
	boost::filesystem::path result(getHomeDir() / "Library/Application Support" / getApplicationName().getUTF8String());
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


boost::filesystem::path MacOSXApplicationPathProvider::getExecutableDir() const {
	ByteArray path;
	uint32_t size = 4096;
	path.resize(size);
	if (_NSGetExecutablePath(path.getData(), &size) == 0) {
		return boost::filesystem::path(path.toString().getUTF8Data()).parent_path();
	}
	else {
		return boost::filesystem::path();
	}
}

}
