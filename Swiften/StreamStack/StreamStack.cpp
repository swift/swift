#include "Swiften/StreamStack/StreamStack.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/StreamStack/StreamLayer.h"

namespace Swift {

StreamStack::StreamStack(boost::shared_ptr<XMPPLayer> xmppLayer, boost::shared_ptr<LowLayer> physicalLayer) : xmppLayer_(xmppLayer), physicalLayer_(physicalLayer) {
	xmppReadSlotConnection_ = physicalLayer_->onDataRead.connect(boost::bind(&XMPPLayer::parseData, xmppLayer_, _1));
	xmppWriteSignalConnection_ = xmppLayer_->onWriteData.connect(boost::bind(&LowLayer::writeData, physicalLayer_, _1));
}

StreamStack::~StreamStack() {
	// Disconnect the write signal connections to break cyclic signal 
	// dependencies. The read signal connections have
	// to remain, since these can be reached from the main event loop.
	xmppWriteSignalConnection_.disconnect();
	foreach(const boost::bsignals::connection& connection, writeSignalConnections_) {
		connection.disconnect();
	}
}

void StreamStack::addLayer(boost::shared_ptr<StreamLayer> newLayer) {
	xmppReadSlotConnection_.disconnect();
	xmppWriteSignalConnection_.disconnect();

	boost::shared_ptr<LowLayer> lowLayer = (layers_.empty() ? physicalLayer_ : *layers_.rbegin());

	lowLayer->onDataRead.connect(boost::bind(&HighLayer::handleDataRead, newLayer, _1), boost::bsignals::at_front);
	writeSignalConnections_.push_back(newLayer->onWriteData.connect(boost::bind(&LowLayer::writeData, lowLayer, _1), boost::bsignals::at_front));
	xmppWriteSignalConnection_ = xmppLayer_->onWriteData.connect(boost::bind(&LowLayer::writeData, newLayer, _1), boost::bsignals::at_front);
	xmppReadSlotConnection_ = newLayer->onDataRead.connect(boost::bind(&XMPPLayer::parseData, xmppLayer_, _1), boost::bsignals::at_front);
	layers_.push_back(newLayer);
}

}
