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
				ReadError
			};

			Connection(const String& domain) : domain_(domain) {}
			virtual ~Connection() {}

			virtual void connect() = 0;
			virtual void disconnect() = 0;
			virtual void write(const ByteArray& data) = 0;

		public:
			boost::signal<void ()> onConnected;
			boost::signal<void (Error)> onError;
			boost::signal<void (const ByteArray&)> onDataRead;

		protected:
			const String& getDomain() const {
				return domain_;
			}

		private:
			String domain_;
	};
}

#endif
