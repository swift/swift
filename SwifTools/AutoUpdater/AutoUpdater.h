/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
	class AutoUpdater {
		public:
			virtual ~AutoUpdater();

			virtual void checkForUpdates() = 0;
	};
}
