/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
    class CrashReporter {
        public:
            CrashReporter(const boost::filesystem::path& path);

        private:
            struct Private;
            boost::shared_ptr<Private> p;
    };
}
