/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/StreamStack.h>

#include <boost/bind.hpp>

#include <Swiften/StreamStack/LowLayer.h>
#include <Swiften/StreamStack/StreamLayer.h>
#include <Swiften/StreamStack/XMPPLayer.h>

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
