#include "SwifTools/Idle/ActualIdleDetector.h"

#include <boost/bind.hpp>

#include "SwifTools/Idle/IdleQuerier.h"
#include "Swiften/Network/Timer.h"
#include "Swiften/Network/TimerFactory.h"

namespace Swift {

ActualIdleDetector::ActualIdleDetector(IdleQuerier* querier, TimerFactory* timerFactory, int refreshRateSeconds) : querier(querier), isIdle(false) {
	timer = timerFactory->createTimer(refreshRateSeconds*1000);
	timer->onTick.connect(boost::bind(&ActualIdleDetector::handleTimerTick, this));
	timer->start();
}

ActualIdleDetector::~ActualIdleDetector() {
	timer->stop();
}

void ActualIdleDetector::handleTimerTick() {
	bool idle = (querier->getIdleTimeSeconds() >= getIdleTimeSeconds());
	if (idle != isIdle) {
		isIdle = idle;
		onIdleChanged(isIdle);
	}
}

}
