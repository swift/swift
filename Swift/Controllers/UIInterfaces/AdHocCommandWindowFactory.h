/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/AdHocCommandWindow.h>
#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

namespace Swift {
	class AdHocCommandWindowFactory {
		public:
			virtual ~AdHocCommandWindowFactory() {}
			/**
			 * The UI should deal with the lifetime of this window (i.e. DeleteOnClose),
			 * so the result isn't returned.
			 */
			virtual void createAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command) = 0;
	};
}
