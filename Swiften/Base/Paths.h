/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API Paths {
        public:
            static boost::filesystem::path getExecutablePath();
    };
}
