/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h>

#include <cassert>

#ifdef HAVE_SPARKLE
#include <SwifTools/AutoUpdater/SparkleAutoUpdater.h>
#endif

namespace Swift {

bool PlatformAutoUpdaterFactory::isSupported() const {
#ifdef HAVE_SPARKLE
    return true;
#else
    return false;
#endif
}

AutoUpdater* PlatformAutoUpdaterFactory::createAutoUpdater(const std::string& appcastURL) {
#ifdef HAVE_SPARKLE
    return new SparkleAutoUpdater(appcastURL);
#else
    (void) appcastURL;
    return nullptr;
#endif
}

}
