/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Application/MacOSXApplicationPathProvider.h"

#include <iostream>

namespace Swift {

MacOSXApplicationPathProvider::MacOSXApplicationPathProvider(const String& name) : ApplicationPathProvider(name) {
}

boost::filesystem::path MacOSXApplicationPathProvider::getSettingsDir() const {
	try {
		boost::filesystem::path result(getHomeDir() / "Library/Application Support" / getApplicationName().getUTF8String());
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	boost::filesystem::create_directory(result);
	return result;
}

boost::filesystem::path MacOSXApplicationPathProvider::getHomeDir() const {
	return boost::filesystem::path(getenv("HOME"));
}

}
