/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Network/TimerFactory.h>

namespace Swift {
	class Watchdog {
		public:
			Watchdog(int timeout, TimerFactory* timerFactory) : timedOut(false) {
				if (timeout > 0) {
					timer = timerFactory->createTimer(timeout);
					timer->start();
					timer->onTick.connect(boost::bind(&Watchdog::handleTimerTick, this));
				}
				else if (timeout == 0) {
					timedOut = true;
				}
			}

			~Watchdog() {
				if (timer) {
					timer->stop();
				}
			}

			bool getTimedOut() const {
				return timedOut;
			}

		private:
			void handleTimerTick() {
				timedOut = true;
			}

		private:
			Timer::ref timer;
			bool timedOut;
	};
}
