#include "Swiften/Base/String.h"

namespace Swift {
	class AutoUpdater;

	class PlatformAutoUpdaterFactory {
		public:
			bool isSupported() const;

			AutoUpdater* createAutoUpdater(const String& appcastURL);
	};
}
