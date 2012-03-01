/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swiften/Parser/XMLParserClient.h>

#include <map>
#include <set>

namespace Swift {

class XMLSettingsProvider : public SettingsProvider, public XMLParserClient {
	public:
		XMLSettingsProvider(const std::string& xmlConfig);
		virtual ~XMLSettingsProvider();
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


		virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
		virtual void handleEndElement(const std::string& element, const std::string& ns);
		virtual void handleCharacterData(const std::string& data);

	protected:
		virtual bool getIsSettingFinal(const std::string& settingPath);
	private:
		std::map<std::string /*settingPath*/, std::string /*settingValue*/> values_;
		/* Settings that are final*/
		std::set<std::string /*settingPath*/> finals_;

		enum Level {
			TopLevel = 0,
			SettingLevel = 2
		};

		int level_;
		std::string currentElement_;
		std::string currentText_;
};

}




