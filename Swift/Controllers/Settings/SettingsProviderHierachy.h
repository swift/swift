/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Settings/SettingsProvider.h>

namespace Swift {

class SettingsProviderHierachy : public SettingsProvider {
	public:
		virtual ~SettingsProviderHierachy();
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
	protected:
		virtual bool getIsSettingFinal(const std::string& settingPath);

	public:
		/**
		 * Adds a provider less significant than any already added.
		 * This means that if an existing provider has a setting, this provider won't be asked.
		 * Any settings will be pushed into the topmost (least significant) provider.
		 * Does not take ownership of provider.
		 */
		void addProviderToTopOfStack(SettingsProvider* provider);
	private:
		SettingsProvider* getWritableProvider();
	private:
		/* Start/Left is most significant (lowest), left overrides right.*/
		std::vector<SettingsProvider*> providers_;
};

}



