/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class AdHocCommandWindow {
		public:
			virtual ~AdHocCommandWindow() {}
			virtual void setOnline(bool /*online*/) {}
			boost::signal<void ()> onClosing;
	};
}
