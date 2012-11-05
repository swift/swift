/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/StatusCache.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/fstream.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/ByteArray.h>
#include <SwifTools/Application/ApplicationPathProvider.h>

namespace Swift {

static const size_t MAX_ENTRIES = 200;

StatusCache::StatusCache(ApplicationPathProvider* paths) {
	paths_ = paths;
	path_ = paths_->getDataDir() / "StatusCache";
	loadRecents();
}

StatusCache::~StatusCache() {

}

std::vector<StatusCache::PreviousStatus> StatusCache::getMatches(const std::string& substring, size_t maxCount) const {
	std::vector<PreviousStatus> matches;
	foreach (const PreviousStatus& status, previousStatuses_) {
		if (substring.empty() || boost::algorithm::ifind_first(status.first, substring)) {
			matches.push_back(status);
			if (matches.size() == maxCount) {
				break;
			}
		}
	}
	return matches;
}

void StatusCache::addRecent(const std::string& text, StatusShow::Type type) {
	if (text.empty()) {
		return;
	}
	for (std::list<PreviousStatus>::iterator i = previousStatuses_.begin(); i != previousStatuses_.end(); ) {
		if ((*i).first == text && (*i).second == type) {
			previousStatuses_.erase(i++);
		}
		else {
			++i;
		}
	}
	previousStatuses_.push_front(PreviousStatus(text, type));
	for (size_t i = previousStatuses_.size(); i > MAX_ENTRIES; i--) {
		previousStatuses_.pop_back();
	}
	saveRecents();
}

void StatusCache::loadRecents() {
	try {
		if (boost::filesystem::exists(path_)) {
			ByteArray data;
			readByteArrayFromFile(data, path_.string());
			std::string stringData = byteArrayToString(data);
			std::vector<std::string> lines;
			boost::split(lines, stringData, boost::is_any_of("\n"));
			foreach (const std::string& line, lines) {
				std::vector<std::string> bits;
				boost::split(bits, line, boost::is_any_of("\t"));
				if (bits.size() < 2) {
					continue;
				}
				StatusShow::Type type;
				type = static_cast<StatusShow::Type>(boost::lexical_cast<size_t>(bits[0]));
				previousStatuses_.push_back(PreviousStatus(bits[1], type));
			}
		}
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

void StatusCache::saveRecents() {
	try {
		if (!boost::filesystem::exists(path_.parent_path())) {
			boost::filesystem::create_directories(path_.parent_path());
		}
		boost::filesystem::ofstream file(path_);
		foreach (const PreviousStatus& recent, previousStatuses_) {
			std::string message = recent.first;
			boost::replace_all(message, "\t", " ");
			file << recent.second << "\t" << message << std::endl;
		}
		file.close();
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

}




