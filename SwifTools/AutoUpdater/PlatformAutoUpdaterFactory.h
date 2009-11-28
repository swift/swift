#include "Swiften/Base/String.h"

namespace Swift {
	class AutoUpdater;

	class PlatformAutoUpdaterFactory {
		public:
			AutoUpdater* createAutoUpdater(const String& appcastURL);
	};
}
