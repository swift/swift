/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/StatusShow.h"

namespace Swift {
	class SystemTray {
		public:
			virtual ~SystemTray(){};
			virtual void setUnreadMessages(bool some) = 0;
			virtual void setStatusType(StatusShow::Type type) = 0;
			virtual void setConnecting() = 0;
	};
}
