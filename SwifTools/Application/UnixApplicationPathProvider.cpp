/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Application/UnixApplicationPathProvider.h>

#include <stdlib.h>

#include <boost/algorithm/string.hpp>

#include <unistd.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>

namespace Swift {

UnixApplicationPathProvider::UnixApplicationPathProvider(const std::string& name) : ApplicationPathProvider(name) {
    resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
    resourceDirs.push_back(getExecutableDir() / ".." / "share" / boost::to_lower_copy(getApplicationName())); // Local install
    char* xdgDataDirs = getenv("XDG_DATA_DIRS");
    if (xdgDataDirs) {
        std::vector<std::string> dataDirs = String::split(xdgDataDirs, ':');
        if (!dataDirs.empty()) {
            for (const auto& dir : dataDirs) {
                resourceDirs.push_back(boost::filesystem::path(dir) / "swift");
            }
            return;
        }
    }
    resourceDirs.push_back("/usr/local/share/" + boost::to_lower_copy(getApplicationName()));
    resourceDirs.push_back("/usr/share/" + boost::to_lower_copy(getApplicationName()));
}

boost::filesystem::path UnixApplicationPathProvider::getHomeDir() const {
    char* home = getenv("HOME");
    return home ? boost::filesystem::path(home) : boost::filesystem::path();
}

boost::filesystem::path UnixApplicationPathProvider::getDataDir() const {
    char* xdgDataHome = getenv("XDG_DATA_HOME");
    std::string dataDir;
    if (xdgDataHome) {
        dataDir = std::string(xdgDataHome);
    }

    boost::filesystem::path dataPath = (dataDir.empty() ?
            getHomeDir() / ".local" / "share"
            : boost::filesystem::path(dataDir)) / boost::to_lower_copy(getApplicationName());

    try {
        boost::filesystem::create_directories(dataPath);
    }
    catch (const boost::filesystem::filesystem_error& e) {
        SWIFT_LOG(error) << "file system error: " << e.what() << std::endl;
    }
    return dataPath;
}

}
