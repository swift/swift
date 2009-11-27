#include "SwifTools/AutoUpdater/PlatformAutoUpdaterFactory.h"

#ifdef HAVE_SPARKLE
#include "SwifTools/AutoUpdater/SparkleAutoUpdater.h"
#else
#include "SwifTools/AutoUpdater/DummyAutoUpdater.h"
#endif

namespace Swift {

AutoUpdater* PlatformAutoUpdaterFactory::createAutoUpdater(const String& appcastURL) {
#ifdef HAVE_SPARKLE
	return new SparkleAutoUpdater(appcastURL);
#else
	(void) appcastURL;
	return new DummyAutoUpdater();
#endif
}

}
