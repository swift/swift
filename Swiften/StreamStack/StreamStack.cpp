/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StreamStack/StreamStack.h>

#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/StreamStack/XMPPLayer.h>
#include <Swiften/StreamStack/LowLayer.h>
#include <Swiften/StreamStack/StreamLayer.h>

namespace Swift {

StreamStack::StreamStack(XMPPLayer* xmppLayer, LowLayer* physicalLayer) : xmppLayer_(xmppLayer), physicalLayer_(physicalLayer) {
	physicalLayer_->setParentLayer(xmppLayer_);
	xmppLayer_->setChildLayer(physicalLayer_);
}

StreamStack::~StreamStack() {
}

void StreamStack::addLayer(StreamLayer* newLayer) {
	LowLayer* lowLayer = layers_.empty() ? physicalLayer_ : *layers_.rbegin();

	xmppLayer_->setChildLayer(newLayer);
	newLayer->setParentLayer(xmppLayer_);
	
	lowLayer->setParentLayer(newLayer);
	newLayer->setChildLayer(lowLayer);

	layers_.push_back(newLayer);
}

}
