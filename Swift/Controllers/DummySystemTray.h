/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/SystemTray.h"

namespace Swift {
	class DummySystemTray : public SystemTray {
	public:
		void setUnreadMessages(bool some) {};
		void setStatusType(StatusShow::Type type) {};
		void setConnecting() {}
	};
}
