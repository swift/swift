#include "Swiften/StreamStack/WhitespacePingLayer.h"
#include "Swiften/Network/Timer.h"

namespace Swift {

static const int TIMEOUT_MILLISECONDS = 60000;

WhitespacePingLayer::WhitespacePingLayer() {
	timer = boost::shared_ptr<Timer>(new Timer(TIMEOUT_MILLISECONDS));
	timer->onTick.connect(boost::bind(&WhitespacePingLayer::handleTimerTick, this));
	timer->start();
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

}
