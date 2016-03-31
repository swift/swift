/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/StatusCache.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/foreach.h>

#include <SwifTools/Application/ApplicationPathProvider.h>

namespace lambda = boost::lambda;

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
        if (substring.empty() || (boost::algorithm::ifind_first(status.first, substring) && substring != status.first)) {
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
    previousStatuses_.remove_if(lambda::bind(&PreviousStatus::first, lambda::_1) == text && lambda::bind(&PreviousStatus::second, lambda::_1) == type);
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
            readByteArrayFromFile(data, path_);
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
                previousStatuses_.push_back(PreviousStatus(boost::trim_copy(bits[1]), type));
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




