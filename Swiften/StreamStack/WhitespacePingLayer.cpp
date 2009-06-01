#include "Swiften/StreamStack/WhitespacePingLayer.h"
#include "Swiften/Network/Timer.h"

namespace Swift {

static const int TIMEOUT_MILLISECONDS = 60000;

WhitespacePingLayer::WhitespacePingLayer() {
	timer_ = new Timer(TIMEOUT_MILLISECONDS);
	timer_->onTick.connect(boost::bind(&WhitespacePingLayer::handleTimerTick, this));
	timer_->start();
}

WhitespacePingLayer::~WhitespacePingLayer() {
	delete timer_;
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
