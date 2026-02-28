/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Platform.h>

namespace Swift {

namespace UpdateFeeds {
    const std::string StableChannel = "stable";
    const std::string TestingChannel = "testing";
    const std::string DevelopmentChannel = "development";

#if defined(SWIFTEN_PLATFORM_MACOSX)
    const std::string StableAppcastFeed = "https://swift.im/downloads/swift-stable-appcast-mac.xml";
    const std::string TestingAppcastFeed = "https://swift.im/downloads/swift-testing-appcast-mac.xml";
    const std::string DevelopmentAppcastFeed = "https://swift.im/downloads/swift-development-appcast-mac.xml";
#else
    const std::string StableAppcastFeed = "";
    const std::string TestingAppcastFeed = "";
    const std::string DevelopmentAppcastFeed = "";
#endif
}

}
