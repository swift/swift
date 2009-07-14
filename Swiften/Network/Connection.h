#ifndef SWIFTEN_CONNECTION_H
#define SWIFTEN_CONNECTION_H

#include <boost/signals.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class Connection {
		public:
			enum Error {
				DomainNameResolveError,
				ConnectionError,
				ReadError,
				WriteError
			};

			Connection() {}
			virtual ~Connection() {}

			virtual void listen() = 0;
			virtual void connect(const String& domain) = 0;
			virtual void disconnect() = 0;
			virtual void write(const ByteArray& data) = 0;

		public:
			boost::signal<void ()> onConnected;
			boost::signal<void (Error)> onError;
			boost::signal<void (const ByteArray&)> onDataRead;
	};
}

#endif
