/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSettingsProvider.h"
#include "QtSwiftUtil.h"

#include <QStringList>

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

std::vector<String> QtSettingsProvider::getAvailableProfiles() {
	std::vector<String> profiles;
	QVariant profilesVariant = settings_.value("profileList");
	foreach(QString profileQString, profilesVariant.toStringList()) {
		profiles.push_back(Q2PSTRING(profileQString));
	}
	return profiles;
}

void QtSettingsProvider::createProfile(const String& profile) {
	QStringList stringList = settings_.value("profileList").toStringList();
	stringList.append(P2QSTRING(profile));
	settings_.setValue("profileList", stringList);
}

QSettings* QtSettingsProvider::getQSettings() {
	return &settings_;
}

}

