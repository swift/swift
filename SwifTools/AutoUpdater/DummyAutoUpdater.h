#pragma once

#include "SwifTools/AutoUpdater/AutoUpdater.h"

namespace Swift {
	class DummyAutoUpdater : public AutoUpdater {
		public:
			void checkForUpdates() {}
	};
}
