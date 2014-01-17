/*
 * Copyright (c) 2011-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <algorithm>

#include <Swiften/Network/TimerFactory.h>

namespace Swift {
	class Watchdog {
		public:
			Watchdog(int timeout, TimerFactory* timerFactory);
			~Watchdog();

			bool getTimedOut() const {
				return timedOut;
			}

		private:
			void handleTimerTick();

		private:
			Timer::ref timer;
			int remainingTime;
			TimerFactory* timerFactory;
			bool timedOut;
	};
}
