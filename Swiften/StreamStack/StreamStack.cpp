/*
 * Copyright (c) 2010-2018 Isode Limited.
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

void StreamStack::addLayer(std::unique_ptr<StreamLayer> streamLayer) {
    LowLayer* lowLayer = layers_.empty() ? physicalLayer_ : layers_.rbegin()->get();

    xmppLayer_->setChildLayer(streamLayer.get());
    streamLayer->setParentLayer(xmppLayer_);

    lowLayer->setParentLayer(streamLayer.get());
    streamLayer->setChildLayer(lowLayer);

    layers_.emplace_back(std::move(streamLayer));
}

}
