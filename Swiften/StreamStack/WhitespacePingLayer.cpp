/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StreamStack/WhitespacePingLayer.h>

#include <boost/bind.hpp>

#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/Timer.h>

namespace Swift {

static const int TIMEOUT_MILLISECONDS = 60000;

WhitespacePingLayer::WhitespacePingLayer(TimerFactory* timerFactory) : isActive(false) {
	timer = timerFactory->createTimer(TIMEOUT_MILLISECONDS);
	timer->onTick.connect(boost::bind(&WhitespacePingLayer::handleTimerTick, this));
}

void WhitespacePingLayer::writeData(const SafeByteArray& data) {
	writeDataToChildLayer(data);
}

void WhitespacePingLayer::handleDataRead(const SafeByteArray& data) {
	writeDataToParentLayer(data);
}

void WhitespacePingLayer::handleTimerTick() {
	timer->stop();
	writeDataToChildLayer(createSafeByteArray(" "));
	timer->start();
}

void WhitespacePingLayer::setActive() {
	isActive = true;
	timer->start();
}

void WhitespacePingLayer::setInactive() {
	timer->stop();
	isActive = false;
}

}
