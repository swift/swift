/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QtSettingsProvider.h>

#include <QStringList>
#include <QFile>

namespace Swift {

QtSettingsProvider::QtSettingsProvider() {
}

QtSettingsProvider::~QtSettingsProvider() {

}

bool QtSettingsProvider::hasSetting(const std::string& key) {
	return !settings_.value(key.c_str()).isNull();
}


std::string QtSettingsProvider::getSetting(const Setting<std::string>& setting) {
	QVariant variant = settings_.value(setting.getKey().c_str());
	return variant.isNull() ? setting.getDefaultValue() : std::string(variant.toString().toUtf8());
}

void QtSettingsProvider::storeSetting(const Setting<std::string>& setting, const std::string& settingValue) {
	bool changed = false;
	if (getSetting(setting) != settingValue) {
		changed = true;
	}
	settings_.setValue(setting.getKey().c_str(), settingValue.c_str());
	if (changed) {
		onSettingChanged(setting.getKey());
	}
	updatePermissions();
}

bool QtSettingsProvider::getSetting(const Setting<bool>& setting) {
	QVariant variant = settings_.value(setting.getKey().c_str());
	return variant.isNull() ? setting.getDefaultValue() : variant.toBool();
}

void QtSettingsProvider::storeSetting(const Setting<bool>& setting, const bool& settingValue) {
	bool changed = false;
	if (getSetting(setting) != settingValue) {
		changed = true;
	}
	settings_.setValue(setting.getKey().c_str(), settingValue);
	if (changed) {
		onSettingChanged(setting.getKey());
	}
	updatePermissions();
}

int QtSettingsProvider::getSetting(const Setting<int>& setting) {
	QVariant variant = settings_.value(setting.getKey().c_str());
	return variant.isNull() ? setting.getDefaultValue() : variant.toInt();
}

void QtSettingsProvider::storeSetting(const Setting<int>& setting, const int& settingValue) {
	bool changed = false;
	if (getSetting(setting) != settingValue) {
		changed = true;
	}
	settings_.setValue(setting.getKey().c_str(), settingValue);
	if (changed) {
		onSettingChanged(setting.getKey());
	}
	updatePermissions();
}

std::vector<std::string> QtSettingsProvider::getAvailableProfiles() {
	std::vector<std::string> profiles;
	QVariant profilesVariant = settings_.value("profileList");
	foreach(QString profileQString, profilesVariant.toStringList()) {
		profiles.push_back(std::string(profileQString.toUtf8()));
	}
	return profiles;
}

void QtSettingsProvider::createProfile(const std::string& profile) {
	QStringList stringList = settings_.value("profileList").toStringList();
	stringList.append(profile.c_str());
	settings_.setValue("profileList", stringList);
	updatePermissions();
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
	updatePermissions();
}

QSettings* QtSettingsProvider::getQSettings() {
	return &settings_;
}

void QtSettingsProvider::updatePermissions() {
#if !defined(Q_WS_WIN) && !defined(Q_WS_MAC)
	QFile file(settings_.fileName());
	if (file.exists()) {
		file.setPermissions(QFile::ReadOwner|QFile::WriteOwner);
	}
#endif
}

bool QtSettingsProvider::getIsSettingFinal(const std::string& /*settingPath*/) {
	return false;
}

}

