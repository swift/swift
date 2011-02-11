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
		virtual String getStringSetting(const String&) {return "";}
		virtual void storeString(const String &, const String &) {}
		virtual bool getBoolSetting(const String &, bool ) {return true;}
		virtual void storeBool(const String &, bool ) {}
		virtual int getIntSetting(const String &, int ) {return 0;}
		virtual void storeInt(const String &, int ) {}
		virtual std::vector<String> getAvailableProfiles() {return std::vector<String>();}
		virtual void createProfile(const String& ) {}
		virtual void removeProfile(const String& ) {}
};

}



