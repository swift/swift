/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/StreamStack.h>

#include <boost/bind.hpp>

#include <Swiften/StreamStack/HighLayer.h>
#include <Swiften/StreamStack/LowLayer.h>
#include <Swiften/StreamStack/StreamLayer.h>

namespace Swift {

StreamStack::StreamStack(std::unique_ptr<HighLayer> topLayer, std::unique_ptr<LowLayer> bottomLayer) : topLayer_(std::move(topLayer)), bottomLayer_(std::move(bottomLayer)) {
    bottomLayer_->setParentLayer(topLayer_.get());
    topLayer_->setChildLayer(bottomLayer_.get());
}

StreamStack::~StreamStack() {
}

void StreamStack::addLayer(std::unique_ptr<StreamLayer> streamLayer) {
    auto* lowLayer = layers_.empty() ? bottomLayer_.get() : layers_.rbegin()->get();

    topLayer_->setChildLayer(streamLayer.get());
    streamLayer->setParentLayer(topLayer_.get());

    lowLayer->setParentLayer(streamLayer.get());
    streamLayer->setChildLayer(lowLayer);

    layers_.emplace_back(std::move(streamLayer));
}

}
