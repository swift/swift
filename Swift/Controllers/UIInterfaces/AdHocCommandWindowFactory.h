/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
