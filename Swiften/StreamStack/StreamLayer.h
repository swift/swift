/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/StreamStack/LowLayer.h>
#include <Swiften/StreamStack/HighLayer.h>

namespace Swift {
	class StreamLayer : public LowLayer, public HighLayer {
		public:
			StreamLayer() {}
	};
}
