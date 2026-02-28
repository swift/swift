/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Path.h>

#include <Swiften/Base/Platform.h>
#include <Swiften/Base/String.h>

using namespace Swift;

boost::filesystem::path Swift::stringToPath(const std::string& path) {
#ifdef SWIFTEN_PLATFORM_WINDOWS
    return boost::filesystem::path(convertStringToWString(path));
#else
    return boost::filesystem::path(path);
#endif
}

std::string Swift::pathToString(const boost::filesystem::path& path) {
#ifdef SWIFTEN_PLATFORM_WINDOWS
    return convertWStringToString(path.native());
#else
    return path.native();
#endif
}
