/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/AdHocCommandWindow.h>
#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

namespace Swift {
class AdHocCommandWindow;
	class AdHocCommandWindowFactory {
		public:
			virtual ~AdHocCommandWindowFactory() {}
			virtual AdHocCommandWindow* createAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command) = 0;
	};
}
