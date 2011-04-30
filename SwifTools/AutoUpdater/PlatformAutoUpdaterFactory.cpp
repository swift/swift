/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
	return NULL;
#endif
}

}
