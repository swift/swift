#pragma once

#include "Swiften/Base/String.h"
#include "SwifTools/AutoUpdater/AutoUpdater.h"

namespace Swift {
	class SparkleAutoUpdater : public AutoUpdater {
		public:
			SparkleAutoUpdater(const String& url);
			~SparkleAutoUpdater();

			void checkForUpdates();
		
		private:
			class Private;
			Private* d;
	};
}
