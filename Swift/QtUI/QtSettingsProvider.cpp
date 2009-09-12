#include "QtSettingsProvider.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtSettingsProvider::QtSettingsProvider() {
}

QtSettingsProvider::~QtSettingsProvider() {

}

String QtSettingsProvider::getStringSetting(const String &settingPath) {
	QVariant setting = settings_.value(P2QSTRING(settingPath));
	return setting.isNull() ? "" : Q2PSTRING(setting.toString());
}

void QtSettingsProvider::storeString(const String &settingPath, const String &settingValue) {
	settings_.setValue(P2QSTRING(settingPath), P2QSTRING(settingValue));
}

bool QtSettingsProvider::getBoolSetting(const String &settingPath, bool defaultValue) {
	QVariant setting = settings_.value(P2QSTRING(settingPath));
	return setting.isNull() ? defaultValue : setting.toBool();
}

void QtSettingsProvider::storeBool(const String &settingPath, bool settingValue) {
	settings_.setValue(P2QSTRING(settingPath), settingValue);
}

}

