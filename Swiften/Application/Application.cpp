/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Application/Application.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <stdlib.h>

#include "Swiften/Application/ApplicationMessageDisplay.h"

namespace Swift {

Application::Application(const String& name) : name_(name) {
}

Application::~Application() {
}

boost::filesystem::path Application::getSettingsFileName() const {
	return getSettingsDir() / "settings";
}

boost::filesystem::path Application::getAvatarDir() const {
	return getSettingsDir() / "avatars";
}

boost::filesystem::path Application::getProfileDir(const String& profile) const {
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
