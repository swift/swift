/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

class Storage {
	public:
		Storage(const std::string& path);
		Storage(const boost::filesystem::path& path);
		void saveSetting(const std::string& setting, const std::string& value);
		std::string getSetting(const std::string& setting);
	private:
		void load();
		std::map<std::string, std::string> settings_;
		std::string path_;
		boost::filesystem::path settingsPath_;
};


