/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/ProfileSettingsProvider.h"

namespace Swift {

ProfileSettingsProvider::ProfileSettingsProvider(const std::string& profile, SettingsProvider* provider) :
	profile_(profile) {
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
}

ProfileSettingsProvider::~ProfileSettingsProvider() {
}

std::string ProfileSettingsProvider::getStringSetting(const std::string &settingPath) {
	//FIXME: Remove shim
	SettingsProvider::Setting<std::string> setting(profileSettingPath(settingPath), "");
	return provider_->getSetting(setting);
}

void ProfileSettingsProvider::storeString(const std::string &settingPath, const std::string &settingValue) {
	//FIXME: Remove shim
	if (!getIsSettingFinal(settingPath)) {
		SettingsProvider::Setting<std::string> setting(profileSettingPath(settingPath), "");
		provider_->storeSetting(setting, settingValue);
	}
}

int ProfileSettingsProvider::getIntSetting(const std::string& settingPath, int defaultValue) {
	//FIXME: Remove shim
	SettingsProvider::Setting<int> setting(profileSettingPath(settingPath), defaultValue);
	return provider_->getSetting(setting);
}
void ProfileSettingsProvider::storeInt(const std::string& settingPath, int settingValue) {
	//FIXME: Remove shim
	if (!getIsSettingFinal(settingPath)) {
		SettingsProvider::Setting<int> setting(profileSettingPath(settingPath), 0);
		provider_->storeSetting(setting, settingValue);
	}
}

bool ProfileSettingsProvider::getIsSettingFinal(const std::string& settingPath) {
	//FIXME: Remove shim
	SettingsProvider::Setting<int> setting(settingPath, 0);
	return provider_->getIsSettingFinal(setting);
}


std::string ProfileSettingsProvider::profileSettingPath(const std::string &settingPath) {
	return profile_ + ":" + settingPath;
}


}

