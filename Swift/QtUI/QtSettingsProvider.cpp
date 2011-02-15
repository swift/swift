/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSettingsProvider.h"

#include <QStringList>

namespace Swift {

QtSettingsProvider::QtSettingsProvider() {
}

QtSettingsProvider::~QtSettingsProvider() {

}

std::string QtSettingsProvider::getStringSetting(const std::string &settingPath) {
	QVariant setting = settings_.value(settingPath.c_str());
	return setting.isNull() ? "" : std::string(setting.toString().toUtf8());
}

void QtSettingsProvider::storeString(const std::string &settingPath, const std::string &settingValue) {
	settings_.setValue(settingPath.c_str(), settingValue.c_str());
}

bool QtSettingsProvider::getBoolSetting(const std::string &settingPath, bool defaultValue) {
	QVariant setting = settings_.value(settingPath.c_str());
	return setting.isNull() ? defaultValue : setting.toBool();
}

void QtSettingsProvider::storeBool(const std::string &settingPath, bool settingValue) {
	settings_.setValue(settingPath.c_str(), settingValue);
}

int QtSettingsProvider::getIntSetting(const std::string &settingPath, int defaultValue) {
	QVariant setting = settings_.value(settingPath.c_str());
	return setting.isNull() ? defaultValue : setting.toInt();
}

void QtSettingsProvider::storeInt(const std::string &settingPath, int settingValue) {
	settings_.setValue(settingPath.c_str(), settingValue);
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
	stringList.append(profile.c_str());
	settings_.setValue("profileList", stringList);
}

void QtSettingsProvider::removeProfile(const std::string& profile) {
	QString profileStart(QString(profile.c_str()) + ":");
	foreach (QString key, settings_.allKeys()) {
		if (key.startsWith(profileStart)) {
			settings_.remove(key);
		}
	}
	QStringList stringList = settings_.value("profileList").toStringList();
	stringList.removeAll(profile.c_str());
	settings_.setValue("profileList", stringList);
}

QSettings* QtSettingsProvider::getQSettings() {
	return &settings_;
}

}

