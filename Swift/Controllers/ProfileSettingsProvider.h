/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

class ProfileSettingsProvider {
	public:
		ProfileSettingsProvider(const std::string& profile, SettingsProvider* provider);
		virtual ~ProfileSettingsProvider();
		virtual std::string getStringSetting(const std::string &settingPath);
		virtual void storeString(const std::string &settingPath, const std::string &settingValue);
		virtual int getIntSetting(const std::string& settingPath, int defaultValue);
		virtual void storeInt(const std::string& settingPath, int settingValue);
		/** See @SettingsProvider::getIsSettingFinal for discussion of what this means.*/
		virtual bool getIsSettingFinal(const std::string& settingPath);
		
	private:
		std::string profileSettingPath(const std::string &settingPath);
		SettingsProvider* provider_; 
		std::string profile_;
};

}


