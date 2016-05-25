/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Application/MacOSXApplicationPathProvider.h>

#include <mach-o/dyld.h>

#include <Swiften/Base/Log.h>

namespace Swift {

MacOSXApplicationPathProvider::MacOSXApplicationPathProvider(const std::string& name) : ApplicationPathProvider(name) {
    resourceDirs.push_back(getExecutableDir() / "../Resources");
    resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
}

boost::filesystem::path MacOSXApplicationPathProvider::getDataDir() const {
    boost::filesystem::path result(getHomeDir() / "Library/Application Support" / getApplicationName());
    try {
        boost::filesystem::create_directory(result);
    }
    catch (const boost::filesystem::filesystem_error& e) {
        SWIFT_LOG(error) << e.what() << std::endl;
    }
    return result;
}

boost::filesystem::path MacOSXApplicationPathProvider::getHomeDir() const {
    return boost::filesystem::path(getenv("HOME"));
}

}
