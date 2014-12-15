/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/LowLayer.h>

#include <cassert>

#include <Swiften/StreamStack/HighLayer.h>

namespace Swift {

LowLayer::LowLayer() : parentLayer(NULL) {
}

LowLayer::~LowLayer() {
}

void LowLayer::writeDataToParentLayer(const SafeByteArray& data) {
	assert(parentLayer);
	parentLayer->handleDataRead(data);
}

}
