/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtSettingsProvider_H
#define SWIFT_QtSettingsProvider_H

#include "Swift/Controllers/Settings/SettingsProvider.h"

#include <QSettings>

namespace Swift {

class QtSettingsProvider : public SettingsProvider {
	public:
		QtSettingsProvider();
		virtual ~QtSettingsProvider();
		virtual std::string getStringSetting(const std::string &settingPath);
		virtual void storeString(const std::string &settingPath, const std::string &settingValue);
		virtual bool getBoolSetting(const std::string &settingPath, bool defaultValue);
		virtual void storeBool(const std::string &settingPath, bool settingValue);
		virtual int getIntSetting(const std::string &settingPath, int defaultValue);
		virtual void storeInt(const std::string &settingPath, int settingValue);
		virtual std::vector<std::string> getAvailableProfiles();
		virtual void createProfile(const std::string& profile);
		virtual void removeProfile(const std::string& profile);
		QSettings* getQSettings();
	
	private:
		void updatePermissions();

	private:
		QSettings settings_;
};

}
#endif



