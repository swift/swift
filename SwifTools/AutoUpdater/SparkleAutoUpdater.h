/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
