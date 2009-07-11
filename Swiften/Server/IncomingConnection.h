#pragma once

#include <boost/signals.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class IncomingConnection {
		public:
			enum Error {
				ConnectionError,
				ReadError
			};

			virtual ~IncomingConnection();

			virtual void write(const ByteArray& data) = 0;

		public:
			boost::signal<void (Error)> onError;
			boost::signal<void (const ByteArray&)> onDataRead;
	};
}
