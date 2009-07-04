#include "Swiften/Application/Application.h"

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
	boost::filesystem::create_directory(result);
	return result;
}

}
