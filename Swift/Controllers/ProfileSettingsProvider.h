/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Settings/SettingsProvider.h"

namespace Swift {

class ProfileSettingsProvider {
	public:
		ProfileSettingsProvider(const String& profile, SettingsProvider* provider) : profile_(profile) {
			provider_ = provider;
			bool found = false;
			foreach (String existingProfile, provider->getAvailableProfiles()) {
				if (existingProfile == profile) {
					found = true;
				}
			}
			if (!found) {
				provider_->createProfile(profile);
			}
		};
		virtual ~ProfileSettingsProvider() {};
		virtual String getStringSetting(const String &settingPath) {return provider_->getStringSetting(profileSettingPath(settingPath));};
		virtual void storeString(const String &settingPath, const String &settingValue) {provider_->storeString(profileSettingPath(settingPath), settingValue);};
		virtual int getIntSetting(const String& settingPath, int defaultValue) {return provider_->getIntSetting(settingPath, defaultValue);}
		virtual void storeInt(const String& settingPath, int settingValue) {provider_->storeInt(settingPath, settingValue);}
		
	private:
		String profileSettingPath(const String &settingPath) {return profile_ + ":" + settingPath;};
		SettingsProvider* provider_; 
		String profile_;
};

}


