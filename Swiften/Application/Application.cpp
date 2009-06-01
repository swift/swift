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

boost::filesystem::path Application::getHomeDir() const {
	// FIXME: Does this work on windows?
	char* homeDirRaw = getenv("HOME");
	boost::filesystem::path homeDir(homeDirRaw);
	return homeDir;
}

}
