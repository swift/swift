/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Settings/SettingsProvider.h"

namespace Swift {

class ProfileSettingsProvider {
	public:
		ProfileSettingsProvider(const std::string& profile, SettingsProvider* provider) : profile_(profile) {
			provider_ = provider;
			bool found = false;
			foreach (std::string existingProfile, provider->getAvailableProfiles()) {
				if (existingProfile == profile) {
					found = true;
				}
			}
			if (!found) {
				provider_->createProfile(profile);
			}
		};
		virtual ~ProfileSettingsProvider() {};
		virtual std::string getStringSetting(const std::string &settingPath) {return provider_->getStringSetting(profileSettingPath(settingPath));};
		virtual void storeString(const std::string &settingPath, const std::string &settingValue) {provider_->storeString(profileSettingPath(settingPath), settingValue);};
		virtual int getIntSetting(const std::string& settingPath, int defaultValue) {return provider_->getIntSetting(settingPath, defaultValue);}
		virtual void storeInt(const std::string& settingPath, int settingValue) {provider_->storeInt(settingPath, settingValue);}
		
	private:
		std::string profileSettingPath(const std::string &settingPath) {return profile_ + ":" + settingPath;};
		SettingsProvider* provider_; 
		std::string profile_;
};

}


