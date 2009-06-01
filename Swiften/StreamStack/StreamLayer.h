#ifndef SWIFTEN_STREAMLAYER_H
#define SWIFTEN_STREAMLAYER_H

#include <boost/signal.hpp>

#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/StreamStack/HighLayer.h"

namespace Swift {
	class StreamLayer : public LowLayer, public HighLayer {
		public:
			StreamLayer() {}
	};
}

#endif
