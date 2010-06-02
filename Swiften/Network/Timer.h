/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_signalslib.h"

namespace Swift {
	class Timer {
		public:
			virtual ~Timer();

			virtual void start() = 0;
			virtual void stop() = 0;

			boost::signal<void ()> onTick;
	};
}
