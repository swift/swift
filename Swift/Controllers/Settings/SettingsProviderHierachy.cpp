/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>

#include <Swiften/Base/Log.h>

namespace Swift {

SettingsProviderHierachy::~SettingsProviderHierachy() {
}

bool SettingsProviderHierachy::hasSetting(const std::string& key) {
    for (auto provider : providers_) {
        if (provider->hasSetting(key)) {
            return true;
        }
    }
    return false;
}

std::string SettingsProviderHierachy::getSetting(const Setting<std::string>& setting) {
    std::string value = setting.getDefaultValue();
    for (auto provider : providers_) {
        std::string providerSetting = provider->getSetting(setting);
        if (provider->hasSetting(setting.getKey())) {
            value = providerSetting;
        }
        if (provider->getIsSettingFinal(setting.getKey())) {
            return value;
        }
    }
    return value;
}

void SettingsProviderHierachy::storeSetting(const Setting<std::string>& setting, const std::string& settingValue) {
    if (!getIsSettingFinal(setting.getKey())) {
        getWritableProvider()->storeSetting(setting, settingValue);
    }
}

bool SettingsProviderHierachy::getSetting(const Setting<bool>& setting) {
    bool value = setting.getDefaultValue();
    for (auto provider : providers_) {
        bool providerSetting = provider->getSetting(setting);
        if (provider->hasSetting(setting.getKey())) {
            value = providerSetting;
            if (provider->getIsSettingFinal(setting.getKey())) {
                return providerSetting;
            }
        }
    }
    return value;
}

void SettingsProviderHierachy::storeSetting(const Setting<bool>& setting, const bool& settingValue) {
    if (!getIsSettingFinal(setting.getKey())) {
        getWritableProvider()->storeSetting(setting, settingValue);
    }
}

int SettingsProviderHierachy::getSetting(const Setting<int>& setting) {
    int value = setting.getDefaultValue();
    for (auto provider : providers_) {
        int providerSetting = provider->getSetting(setting);
        if (provider->hasSetting(setting.getKey())) {
            value = providerSetting;
            if (provider->getIsSettingFinal(setting.getKey())) {
                return providerSetting;
            }
        }
    }
    return value;
}

void SettingsProviderHierachy::storeSetting(const Setting<int>& setting, const int& settingValue) {
    if (!getIsSettingFinal(setting.getKey())) {
        getWritableProvider()->storeSetting(setting, settingValue);
    }
}

std::vector<std::string> SettingsProviderHierachy::getAvailableProfiles() {
    /* Always pull profiles from the topmost */
    return getWritableProvider()->getAvailableProfiles();
}

void SettingsProviderHierachy::createProfile(const std::string& profile) {
    return getWritableProvider()->createProfile(profile);
}

void SettingsProviderHierachy::removeProfile(const std::string& profile) {
    return getWritableProvider()->removeProfile(profile);
}

bool SettingsProviderHierachy::getIsSettingFinal(const std::string& settingPath) {
    bool isFinal = false;
    for (auto provider : providers_) {
        isFinal |= provider->getIsSettingFinal(settingPath);
    }
    return isFinal;
}

SettingsProvider* SettingsProviderHierachy::getWritableProvider() {
    return providers_.back();
}

void SettingsProviderHierachy::addProviderToTopOfStack(SettingsProvider* provider) {
    providers_.push_back(provider);
    provider->onSettingChanged.connect(onSettingChanged);
}

}

