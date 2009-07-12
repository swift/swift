#pragma once

#include <boost/signal.hpp>

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class IncomingConnection {
		public:
			virtual ~IncomingConnection();

			virtual void write(const ByteArray& data) = 0;

			//boost::signal<void (Error)> onError;
			boost::signal<void (const ByteArray&)> onDataRead;
	};
}
