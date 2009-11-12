#include "Swiften/StreamStack/WhitespacePingLayer.h"

#include <boost/bind.hpp>

#include "Swiften/Network/TimerFactory.h"
#include "Swiften/Network/Timer.h"

namespace Swift {

static const int TIMEOUT_MILLISECONDS = 60000;

WhitespacePingLayer::WhitespacePingLayer(TimerFactory* timerFactory) : isActive(false) {
	timer = timerFactory->createTimer(TIMEOUT_MILLISECONDS);
	timer->onTick.connect(boost::bind(&WhitespacePingLayer::handleTimerTick, this));
}

void WhitespacePingLayer::writeData(const ByteArray& data) {
	onWriteData(data);
}

void WhitespacePingLayer::handleDataRead(const ByteArray& data) {
	onDataRead(data);
}

void WhitespacePingLayer::handleTimerTick() {
	onWriteData(" ");
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
