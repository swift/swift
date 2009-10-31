#include "Swiften/StreamStack/WhitespacePingLayer.h"

#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/MainBoostIOServiceThread.h"
#include "Swiften/Network/Timer.h"

namespace Swift {

static const int TIMEOUT_MILLISECONDS = 60000;

WhitespacePingLayer::WhitespacePingLayer() : isActive(false) {
	// FIXME: Create a BoostTimerFactory.
	timer = boost::shared_ptr<Timer>(new Timer(TIMEOUT_MILLISECONDS, &MainBoostIOServiceThread::getInstance().getIOService()));
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
