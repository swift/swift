/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StreamStack/HighLayer.h>

#include <cassert>

#include <Swiften/StreamStack/LowLayer.h>

namespace Swift {

HighLayer::HighLayer() : childLayer(NULL) {
}

HighLayer::~HighLayer() {
}

void HighLayer::writeDataToChildLayer(const SafeByteArray& data) {
	//assert(childLayer);
	if (childLayer) {
		childLayer->writeData(data);
	}
}

}
