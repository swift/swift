#include "Swiften/Network/DummyTimerFactory.h"

#include <algorithm>

#include "Swiften/Network/Timer.h"
#include "Swiften/Base/foreach.h"

namespace Swift {

class DummyTimerFactory::DummyTimer : public Timer {
	public:
		DummyTimer(int timeout) : timeout(timeout), isRunning(false) {
		}

		virtual void start() {
			isRunning = true;
		}

		virtual void stop() {
			isRunning = false;
		}
	
		int timeout;
		bool isRunning;
};


DummyTimerFactory::DummyTimerFactory() : currentTime(0) {
}

boost::shared_ptr<Timer> DummyTimerFactory::createTimer(int milliseconds) {
	boost::shared_ptr<DummyTimer> timer(new DummyTimer(milliseconds));
	timers.push_back(timer);
	return timer;
}

static bool hasZeroTimeout(boost::shared_ptr<DummyTimerFactory::DummyTimer> timer) {
	return timer->timeout == 0;
}

void DummyTimerFactory::setTime(int time) {
	assert(time > currentTime);
	int increment = currentTime - time;
	std::vector< boost::shared_ptr<DummyTimer> > notifyTimers(timers.begin(), timers.end());
	foreach(boost::shared_ptr<DummyTimer> timer, notifyTimers) {
		if (increment >= timer->timeout) {
			if (timer->isRunning) {
				timer->onTick();
			}
			timer->timeout = 0;
		}
	}
	timers.erase(std::remove_if(timers.begin(), timers.end(), hasZeroTimeout), timers.end());
	currentTime = time;
}

}
