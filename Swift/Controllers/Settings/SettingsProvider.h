/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
		virtual int getIntSetting(const String &settingPath, int defaultValue) = 0;
		virtual void storeInt(const String &settingPath, int settingValue) = 0;
		virtual std::vector<String> getAvailableProfiles() = 0;
		virtual void createProfile(const String& profile) = 0;
		virtual void removeProfile(const String& profile) = 0;
};

}
#endif


