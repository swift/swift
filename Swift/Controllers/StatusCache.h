/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>
#include <string>
#include <utility>

#include <boost/filesystem/path.hpp>

#include <Swiften/Elements/StatusShow.h>

namespace Swift {
    class ApplicationPathProvider;

    class StatusCache {
        public:
            typedef std::pair<std::string, StatusShow::Type> PreviousStatus;
        public:
            StatusCache(ApplicationPathProvider* paths);
            ~StatusCache();

            std::vector<PreviousStatus> getMatches(const std::string& substring, size_t maxCount) const;
            void addRecent(const std::string& text, StatusShow::Type type);

        private:
            void saveRecents();
            void loadRecents();

        private:
            boost::filesystem::path path_;
            std::list<PreviousStatus> previousStatuses_;
            ApplicationPathProvider* paths_;
    };
}


