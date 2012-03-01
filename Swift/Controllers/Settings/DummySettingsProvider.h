/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Settings/SettingsProvider.h>

#include <map>

namespace Swift {

class DummySettingsProvider : public SettingsProvider {
	public:
		virtual ~DummySettingsProvider() {}
		virtual std::string getSetting(const Setting<std::string>& setting) {
			return stringValues.find(setting.getKey()) != stringValues.end() ? stringValues[setting.getKey()] : setting.getDefaultValue();
		};
		virtual void storeSetting(const Setting<std::string>& setting, const std::string& value) {
			stringValues[setting.getKey()] = value;
			onSettingChanged(setting.getKey());
		};
		virtual bool getSetting(const Setting<bool>& setting) {
			return boolValues.find(setting.getKey()) != boolValues.end() ? boolValues[setting.getKey()] : setting.getDefaultValue();
		};
		virtual void storeSetting(const Setting<bool>& setting, const bool& value) {
			boolValues[setting.getKey()] = value;
			onSettingChanged(setting.getKey());
		};
		virtual int getSetting(const Setting<int>& setting) {
			return intValues.find(setting.getKey()) != intValues.end() ? intValues[setting.getKey()] : setting.getDefaultValue();
		};
		virtual void storeSetting(const Setting<int>& setting, const int& value) {
			intValues[setting.getKey()] = value;
			onSettingChanged(setting.getKey());
		};
		virtual std::vector<std::string> getAvailableProfiles() {return std::vector<std::string>();}
		virtual void createProfile(const std::string& ) {}
		virtual void removeProfile(const std::string& ) {}
		virtual bool getIsSettingFinal(const std::string& settingPath) {return finals.count(settingPath);}
		void setFinal(const std::string& settingPath) {
			finals.insert(settingPath);
		}
		virtual bool hasSetting(const std::string& key) {
			return stringValues.find(key) != stringValues.end() || intValues.find(key) != intValues.end() || boolValues.find(key) != boolValues.end();
		}
	private:
		std::map<std::string, std::string> stringValues;
		std::map<std::string, int> intValues;
		std::map<std::string, bool> boolValues;
		std::set<std::string> finals;
};

}



