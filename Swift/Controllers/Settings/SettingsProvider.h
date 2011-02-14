/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SettingsProvider_H
#define SWIFTEN_SettingsProvider_H

#include <string>

#include <vector>

namespace Swift {

class SettingsProvider {
	public:
		virtual ~SettingsProvider() {}
		virtual std::string getStringSetting(const std::string &settingPath) = 0;
		virtual void storeString(const std::string &settingPath, const std::string &settingValue) = 0;
		virtual bool getBoolSetting(const std::string &settingPath, bool defaultValue) = 0;
		virtual void storeBool(const std::string &settingPath, bool settingValue) = 0;
		virtual int getIntSetting(const std::string &settingPath, int defaultValue) = 0;
		virtual void storeInt(const std::string &settingPath, int settingValue) = 0;
		virtual std::vector<std::string> getAvailableProfiles() = 0;
		virtual void createProfile(const std::string& profile) = 0;
		virtual void removeProfile(const std::string& profile) = 0;
};

}
#endif


