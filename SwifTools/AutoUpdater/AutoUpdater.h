#pragma once

namespace Swift {
	class AutoUpdater {
		public:
			virtual ~AutoUpdater();

			virtual void checkForUpdates() = 0;
	};
}
