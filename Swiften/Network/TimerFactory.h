/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/Timer.h>

namespace Swift {
	class SWIFTEN_API TimerFactory {
		public:
			virtual ~TimerFactory();

			virtual Timer::ref createTimer(int milliseconds) = 0;
	};
}
