/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

#include <string>
#include <vector>

namespace Swift {

class SettingsProvider {

	public:
		template <typename T>
		class Setting {
			public:
				Setting(const std::string& key, const T& defaultValue) : key(key), defaultValue(defaultValue) {

				}

				const std::string& getKey() const {
					return key;
				}

				const T& getDefaultValue() const {
					return defaultValue;
				}

			private:
				std::string key;
				T defaultValue;
		};

	public:
		virtual ~SettingsProvider() {}
		virtual std::string getSetting(const Setting<std::string>& setting) = 0;
		virtual void storeSetting(const Setting<std::string>& setting, const std::string& value) = 0;
		virtual bool getSetting(const Setting<bool>& setting) = 0;
		virtual void storeSetting(const Setting<bool>& setting, const bool& value) = 0;
		virtual int getSetting(const Setting<int>& setting) = 0;
		virtual void storeSetting(const Setting<int>& setting, const int& value) = 0;

		virtual std::vector<std::string> getAvailableProfiles() = 0;
		virtual void createProfile(const std::string& profile) = 0;
		virtual void removeProfile(const std::string& profile) = 0;
		/** A final setting is one that this settings provider says may not be overriden by lower priority profiles.
		 * e.g. An Administrator-set configuration to disallow saving user passwords could not be overridden by the user.
		 */
		template<typename T>
		bool getIsSettingFinal(const Setting<T>& setting) {
			return getIsSettingFinal(setting.getKey());
		}
		virtual bool hasSetting(const std::string& key) = 0;

		friend class SettingsProviderHierachy;
	protected:
		virtual bool getIsSettingFinal(const std::string& settingPath) = 0;

	public:
		/**
		 * Emitted when a setting is changed.
		 */
		boost::signal<void (const std::string& /*Setting's Path*/)> onSettingChanged;
};

}



