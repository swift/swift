#ifndef SWIFTEN_HIGHLAYER_H
#define SWIFTEN_HIGHLAYER_H

#include <boost/signal.hpp>

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class HighLayer {
		public:
			virtual ~HighLayer();

			virtual void handleDataRead(const ByteArray& data) = 0;

			boost::signal<void (const ByteArray&)> onWriteData;
	};
}

#endif
