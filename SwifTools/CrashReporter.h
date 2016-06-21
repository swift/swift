/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/filesystem.hpp>

namespace Swift {
    class CrashReporter {
        public:
            CrashReporter(const boost::filesystem::path& path, const std::string& dumpPrefix);

        private:
            struct Private;
            std::shared_ptr<Private> p;
    };
}
