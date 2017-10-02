/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/filesystem/path.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    /**
     * Creates a path for the given UTF-8 encoded string.
     * This works independently of global locale settings.
     */
    SWIFTEN_API boost::filesystem::path stringToPath(const std::string&);

    /**
     * Returns the UTF-8 representation of the given path
     * This works independently of global locale settings.
     */
    SWIFTEN_API std::string pathToString(const boost::filesystem::path&);
}


