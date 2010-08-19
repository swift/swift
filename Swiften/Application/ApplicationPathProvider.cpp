/*
 * Copyright (c) 2010 Remko TronÃ§on
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Application/Application.h"

#include <boost/filesystem.hpp>
#include <iostream>

#include "Swiften/Application/ApplicationPathProvider.h"

namespace Swift {

ApplicationPathProvider::ApplicationPathProvider(const String& applicationName) : applicationName(applicationName) {
}

ApplicationPathProvider::~ApplicationPathProvider() {
}

boost::filesystem::path ApplicationPathProvider::getSettingsFileName() const {
	return getSettingsDir() / "settings";
}

boost::filesystem::path ApplicationPathProvider::getAvatarDir() const {
	return getSettingsDir() / "avatars";
}

boost::filesystem::path ApplicationPathProvider::getProfileDir(const String& profile) const {
	boost::filesystem::path result(getHomeDir() / profile.getUTF8String());
	try {
		boost::filesystem::create_directory(result);
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return result;
}

}
