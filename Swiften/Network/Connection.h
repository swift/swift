#pragma once

#include <boost/signals.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class HostAddressPort;

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
			virtual void connect(const HostAddressPort& address) = 0;
			virtual void connect(const String& domain) = 0;
			virtual void disconnect() = 0;
			virtual void write(const ByteArray& data) = 0;

		public:
			boost::signal<void ()> onConnected;
			boost::signal<void (const boost::optional<Error>&)> onDisconnected;
			boost::signal<void (const ByteArray&)> onDataRead;
	};
}
