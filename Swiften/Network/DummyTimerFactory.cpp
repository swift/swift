/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/DummyTimerFactory.h>

#include <algorithm>

#include <Swiften/Base/foreach.h>
#include <Swiften/Network/Timer.h>

namespace Swift {

class DummyTimerFactory::DummyTimer : public Timer {
	public:
		DummyTimer(int timeout, DummyTimerFactory* factory) : timeout(timeout), factory(factory), isRunning(false), startTime(~0) {
		}

		virtual void start() {
			isRunning = true;
			startTime = factory->currentTime;
		}

		virtual void stop() {
			isRunning = false;
		}

		int getAlarmTime() const {
			return startTime + timeout;
		}
	
		int timeout;
		DummyTimerFactory* factory;
		bool isRunning;
		int startTime;
};


DummyTimerFactory::DummyTimerFactory() : currentTime(0) {
}

boost::shared_ptr<Timer> DummyTimerFactory::createTimer(int milliseconds) {
	boost::shared_ptr<DummyTimer> timer(new DummyTimer(milliseconds, this));
	timers.push_back(timer);
	return timer;
}

void DummyTimerFactory::setTime(int time) {
	assert(time > currentTime);
	foreach(boost::shared_ptr<DummyTimer> timer, timers) {
		if (timer->getAlarmTime() > currentTime && timer->getAlarmTime() <= time && timer->isRunning) {
			timer->onTick();
		}
	}
	currentTime = time;
}

}
