#include "SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h"

#include <cassert>

#ifdef HAVE_SPARKLE
#include "SwifTools/AutoUpdater/SparkleAutoUpdater.h"
#endif

namespace Swift {

bool PlatformAutoUpdaterFactory::isSupported() const {
#ifdef HAVE_SPARKLE
	return true;
#else
	return false;
#endif
}

AutoUpdater* PlatformAutoUpdaterFactory::createAutoUpdater(const String& appcastURL) {
#ifdef HAVE_SPARKLE
	return new SparkleAutoUpdater(appcastURL);
#else
	(void) appcastURL;
	return NULL;
#endif
}

}
