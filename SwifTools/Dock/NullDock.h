/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <SwifTools/Dock/Dock.h>

namespace Swift {
	class NullDock : public Dock {
		public:
			NullDock() {}

			virtual void setNumberOfPendingMessages(int) {
			}
	};
}
