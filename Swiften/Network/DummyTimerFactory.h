/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <list>

#include <Swiften/Network/TimerFactory.h>

namespace Swift {
	class DummyTimerFactory : public TimerFactory {
		public:
			class DummyTimer;

			DummyTimerFactory();

			virtual boost::shared_ptr<Timer> createTimer(int milliseconds);
			void setTime(int time);

		private:
			friend class DummyTimer;
			int currentTime;
			std::list<boost::shared_ptr<DummyTimer> > timers;
	};
}
