#include "Swiften/StreamStack/StreamStack.h"

#include <boost/bind.hpp>

#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/StreamStack/StreamLayer.h"

namespace Swift {

StreamStack::StreamStack(XMPPLayer* xmppLayer, LowLayer* physicalLayer) : xmppLayer_(xmppLayer), physicalLayer_(physicalLayer) {
	xmppReadSlotConnection_ = physicalLayer_->onDataRead.connect(boost::bind(&XMPPLayer::parseData, xmppLayer_, _1));
	xmppWriteSignalConnection_ = xmppLayer_->onWriteData.connect(boost::bind(&LowLayer::writeData, physicalLayer_, _1));
}

void StreamStack::addLayer(StreamLayer* newLayer) {
	xmppReadSlotConnection_.disconnect();
	xmppWriteSignalConnection_.disconnect();

	LowLayer* lowLayer = (layers_.empty() ? physicalLayer_ : *layers_.rbegin());

	lowLayer->onDataRead.connect(boost::bind(&HighLayer::handleDataRead, newLayer, _1), boost::bsignals::at_front);
	newLayer->onWriteData.connect(boost::bind(&LowLayer::writeData, lowLayer, _1), boost::bsignals::at_front);
	xmppWriteSignalConnection_ = xmppLayer_->onWriteData.connect(boost::bind(&LowLayer::writeData, newLayer, _1), boost::bsignals::at_front);
	xmppReadSlotConnection_ = newLayer->onDataRead.connect(boost::bind(&XMPPLayer::parseData, xmppLayer_, _1), boost::bsignals::at_front);
	layers_.push_back(newLayer);
}

}
