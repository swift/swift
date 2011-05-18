/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
