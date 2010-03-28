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
