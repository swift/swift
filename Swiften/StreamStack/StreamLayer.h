/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_STREAMLAYER_H
#define SWIFTEN_STREAMLAYER_H

#include "Swiften/Base/boost_signalslib.h"

#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/StreamStack/HighLayer.h"

namespace Swift {
	class StreamLayer : public LowLayer, public HighLayer {
		public:
			StreamLayer() {}
	};
}

#endif
