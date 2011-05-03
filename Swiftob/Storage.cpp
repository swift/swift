/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiftob/Storage.h>

#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/foreach.h>

typedef std::pair<std::string, std::string> Strings;

Storage::Storage(const std::string& path) : settingsPath_(boost::filesystem::path(path)) {
	load();
}

Storage::Storage(const boost::filesystem::path& path) : settingsPath_(path) {
	load();
}

void Storage::load() {
	if (boost::filesystem::exists(settingsPath_)) {
		Swift::ByteArray data;
		Swift::readByteArrayFromFile(data, settingsPath_.string());
		foreach (std::string line, Swift::String::split(Swift::byteArrayToString(data), '\n')) {
			std::pair<std::string, std::string> pair = Swift::String::getSplittedAtFirst(line, '\t');
			settings_[pair.first] = pair.second;
		}
	}
}

void Storage::saveSetting(const std::string& setting, const std::string& value) {
	settings_[setting] = value;
	std::string settingsString;
	foreach(Strings pair, settings_) {
		settingsString += pair.first + '\t' + pair.second + '\n';
	}
	boost::filesystem::ofstream file(settingsPath_);
	file << settingsString;
	file.close();
}

std::string Storage::getSetting(const std::string& setting) {
	return settings_[setting];
}
