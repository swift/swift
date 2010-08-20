/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtSettingsProvider_H
#define SWIFT_QtSettingsProvider_H

#include "Swiften/Settings/SettingsProvider.h"

#include <QSettings>

namespace Swift {

class QtSettingsProvider : public SettingsProvider {
	public:
		QtSettingsProvider();
		virtual ~QtSettingsProvider();
		virtual String getStringSetting(const String &settingPath);
		virtual void storeString(const String &settingPath, const String &settingValue);
		virtual bool getBoolSetting(const String &settingPath, bool defaultValue);
		virtual void storeBool(const String &settingPath, bool settingValue);
		virtual int getIntSetting(const String &settingPath, int defaultValue);
		virtual void storeInt(const String &settingPath, int settingValue);
		virtual std::vector<String> getAvailableProfiles();
		virtual void createProfile(const String& profile);
		QSettings* getQSettings();
	private:
		QSettings settings_;
};

}
#endif



