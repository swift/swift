/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

namespace Swift {

class SettingsProvider;

class ProfileSettingsProvider {
    public:
        ProfileSettingsProvider(const std::string& profile, SettingsProvider* provider);
        virtual ~ProfileSettingsProvider();
        virtual std::string getStringSetting(const std::string &settingPath);
        virtual void storeString(const std::string &settingPath, const std::string &settingValue);
        virtual int getIntSetting(const std::string& settingPath, int defaultValue);
        virtual void storeInt(const std::string& settingPath, int settingValue);
        /** See \ref SettingsProvider::getIsSettingFinal for discussion of what this means.*/
        virtual bool getIsSettingFinal(const std::string& settingPath);

    private:
        std::string profileSettingPath(const std::string &settingPath);

    private:
        SettingsProvider* provider_;
        std::string profile_;
};

}


