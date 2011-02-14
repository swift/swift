/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <string>

namespace Swift {
	class AutoUpdater;

	class PlatformAutoUpdaterFactory {
		public:
			bool isSupported() const;

			AutoUpdater* createAutoUpdater(const std::string& appcastURL);
	};
}
