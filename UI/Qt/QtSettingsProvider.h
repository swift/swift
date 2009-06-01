#ifndef SWIFT_QtSettingsProvider_H
#define SWIFT_QtSettingsProvider_H

#include "Swiften/Settings/SettingsProvider.h"

#include <QSettings>

namespace Swift {

class QtSettingsProvider : public SettingsProvider {
	public:
		QtSettingsProvider();
		virtual ~QtSettingsProvider();
		virtual String getStringSetting(const String &settingPath);
		virtual void storeString(const String &settingPath, const String &settingValue);
	private:
		QSettings settings_;
};

}
#endif



