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

std::string QtSettingsProvider::getStringSetting(const std::string &settingPath) {
	QVariant setting = settings_.value(P2QSTRING(settingPath));
	return setting.isNull() ? "" : Q2PSTRING(setting.toString());
}

void QtSettingsProvider::storeString(const std::string &settingPath, const std::string &settingValue) {
	settings_.setValue(P2QSTRING(settingPath), P2QSTRING(settingValue));
}

bool QtSettingsProvider::getBoolSetting(const std::string &settingPath, bool defaultValue) {
	QVariant setting = settings_.value(P2QSTRING(settingPath));
	return setting.isNull() ? defaultValue : setting.toBool();
}

void QtSettingsProvider::storeBool(const std::string &settingPath, bool settingValue) {
	settings_.setValue(P2QSTRING(settingPath), settingValue);
}

int QtSettingsProvider::getIntSetting(const std::string &settingPath, int defaultValue) {
	QVariant setting = settings_.value(P2QSTRING(settingPath));
	return setting.isNull() ? defaultValue : setting.toInt();
}

void QtSettingsProvider::storeInt(const std::string &settingPath, int settingValue) {
	settings_.setValue(P2QSTRING(settingPath), settingValue);
}

std::vector<std::string> QtSettingsProvider::getAvailableProfiles() {
	std::vector<std::string> profiles;
	QVariant profilesVariant = settings_.value("profileList");
	foreach(QString profileQString, profilesVariant.toStringList()) {
		profiles.push_back(Q2PSTRING(profileQString));
	}
	return profiles;
}

void QtSettingsProvider::createProfile(const std::string& profile) {
	QStringList stringList = settings_.value("profileList").toStringList();
	stringList.append(P2QSTRING(profile));
	settings_.setValue("profileList", stringList);
}

void QtSettingsProvider::removeProfile(const std::string& profile) {
	QString profileStart(P2QSTRING(profile) + ":");
	foreach (QString key, settings_.allKeys()) {
		if (key.startsWith(profileStart)) {
			settings_.remove(key);
		}
	}
	QStringList stringList = settings_.value("profileList").toStringList();
	stringList.removeAll(P2QSTRING(profile));
	settings_.setValue("profileList", stringList);
}

QSettings* QtSettingsProvider::getQSettings() {
	return &settings_;
}

}

