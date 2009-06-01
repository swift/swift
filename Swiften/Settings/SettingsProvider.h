#ifndef SWIFTEN_SettingsProvider_H
#define SWIFTEN_SettingsProvider_H

#include "Swiften/Base/String.h"

namespace Swift {

class SettingsProvider {
	public:
		virtual ~SettingsProvider() {}
		virtual String getStringSetting(const String &settingPath) = 0;
		virtual void storeString(const String &settingPath, const String &settingValue) = 0;
};

}
#endif


