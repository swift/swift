/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Settings/SettingsProvider.h"

namespace Swift {

class DummySettingsProvider : public SettingsProvider {
	public:
		virtual ~DummySettingsProvider() {}
		virtual std::string getStringSetting(const std::string&) {return "";}
		virtual void storeString(const std::string &, const std::string &) {}
		virtual bool getBoolSetting(const std::string &, bool ) {return true;}
		virtual void storeBool(const std::string &, bool ) {}
		virtual int getIntSetting(const std::string &, int ) {return 0;}
		virtual void storeInt(const std::string &, int ) {}
		virtual std::vector<std::string> getAvailableProfiles() {return std::vector<std::string>();}
		virtual void createProfile(const std::string& ) {}
		virtual void removeProfile(const std::string& ) {}
};

}



