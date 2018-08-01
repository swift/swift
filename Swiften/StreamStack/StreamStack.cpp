/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/StreamStack.h>

#include <boost/bind.hpp>

#include <Swiften/StreamStack/LowLayer.h>
#include <Swiften/StreamStack/StreamLayer.h>
#include <Swiften/StreamStack/HighLayer.h>

namespace Swift {

StreamStack::StreamStack(HighLayer* topLayer, LowLayer* bottomLayer) : topLayer_(topLayer), bottomLayer_(bottomLayer) {
    bottomLayer_->setParentLayer(topLayer_);
    topLayer_->setChildLayer(bottomLayer_);
}

StreamStack::~StreamStack() {
}

void StreamStack::addLayer(std::unique_ptr<StreamLayer> streamLayer) {
    LowLayer* lowLayer = layers_.empty() ? bottomLayer_ : layers_.rbegin()->get();

    topLayer_->setChildLayer(streamLayer.get());
    streamLayer->setParentLayer(topLayer_);

    lowLayer->setParentLayer(streamLayer.get());
    streamLayer->setChildLayer(lowLayer);

    layers_.emplace_back(std::move(streamLayer));
}

}
