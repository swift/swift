/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
