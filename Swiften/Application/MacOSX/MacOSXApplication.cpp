/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Application/MacOSX/MacOSXApplication.h"

namespace Swift {

MacOSXApplication::MacOSXApplication(const String& name) : Application(name) {
}

ApplicationMessageDisplay* MacOSXApplication::getApplicationMessageDisplay() {
	return &messageDisplay_;
}

boost::filesystem::path MacOSXApplication::getSettingsDir() const {
	boost::filesystem::path result(getHomeDir() / "Library/Application Support" / getName().getUTF8String());
	boost::filesystem::create_directory(result);
	return result;
}

boost::filesystem::path MacOSXApplication::getHomeDir() const {
	return boost::filesystem::path(getenv("HOME"));
}

}
