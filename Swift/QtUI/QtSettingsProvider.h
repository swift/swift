/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QSettings>

#include <Swift/Controllers/Settings/SettingsProvider.h>

namespace Swift {

class QtSettingsProvider : public SettingsProvider {
    public:
        QtSettingsProvider();
        virtual ~QtSettingsProvider();
        virtual std::string getSetting(const Setting<std::string>& setting);
        virtual void storeSetting(const Setting<std::string>& setting, const std::string& value);
        virtual bool getSetting(const Setting<bool>& setting);
        virtual void storeSetting(const Setting<bool>& setting, const bool& value);
        virtual int getSetting(const Setting<int>& setting);
        virtual void storeSetting(const Setting<int>& setting, const int& value);
        virtual std::vector<std::string> getAvailableProfiles();
        virtual void createProfile(const std::string& profile);
        virtual void removeProfile(const std::string& profile);
        virtual bool hasSetting(const std::string& key);
        QSettings* getQSettings();
    protected:
        virtual bool getIsSettingFinal(const std::string& settingPath);

    private:
        void updatePermissions();

    private:
        QSettings settings_;
};

}




