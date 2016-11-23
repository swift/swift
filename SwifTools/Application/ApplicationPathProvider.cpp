/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Application/ApplicationPathProvider.h>

#include <boost/filesystem.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Paths.h>

namespace Swift {

ApplicationPathProvider::ApplicationPathProvider(const std::string& applicationName) : applicationName(applicationName) {
}

ApplicationPathProvider::~ApplicationPathProvider() {
}

boost::filesystem::path ApplicationPathProvider::getProfileDir(const std::string& profile) const {
    boost::filesystem::path result(getHomeDir() / profile);
    try {
        boost::filesystem::create_directory(result);
    }
    catch (const boost::filesystem::filesystem_error& e) {
        SWIFT_LOG(error) << e.what() << std::endl;
    }
    return result;
}

boost::filesystem::path ApplicationPathProvider::getResourcePath(const std::string& resource) const {
    std::vector<boost::filesystem::path> resourcePaths = getResourceDirs();
    for (const auto& resourcePath : resourcePaths) {
        boost::filesystem::path r(resourcePath / resource);
        if (boost::filesystem::exists(r)) {
            return r;
        }
    }
    return boost::filesystem::path();
}

boost::filesystem::path ApplicationPathProvider::getExecutableDir() const {
    return Paths::getExecutablePath();
}

}
