#ifndef SWIFTEN_LOWLAYER_H
#define SWIFTEN_LOWLAYER_H

#include <boost/signal.hpp>

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class LowLayer {
		public:
			virtual ~LowLayer();

			virtual void writeData(const ByteArray& data) = 0;

			boost::signal<void (const ByteArray&)> onDataRead;
	};
}

#endif
