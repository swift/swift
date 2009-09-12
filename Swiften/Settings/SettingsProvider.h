#ifndef SWIFTEN_SettingsProvider_H
#define SWIFTEN_SettingsProvider_H

#include "Swiften/Base/String.h"

#include <vector>

namespace Swift {

class SettingsProvider {
	public:
		virtual ~SettingsProvider() {}
		virtual String getStringSetting(const String &settingPath) = 0;
		virtual void storeString(const String &settingPath, const String &settingValue) = 0;
		virtual bool getBoolSetting(const String &settingPath, bool defaultValue) = 0;
		virtual void storeBool(const String &settingPath, bool settingValue) = 0;
		virtual std::vector<String> getAvailableProfiles() = 0;
		virtual void createProfile(const String& profile) = 0;
};

}
#endif


