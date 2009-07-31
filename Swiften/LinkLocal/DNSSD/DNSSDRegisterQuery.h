#pragma once

#include <boost/signal.hpp>
#include <boost/optional.hpp>

#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"

namespace Swift {
	class ByteArray;

	class DNSSDRegisterQuery {
		public:
			virtual ~DNSSDRegisterQuery();

			virtual void registerService() = 0;
			virtual void unregisterService() = 0;
			virtual void updateServiceInfo(const ByteArray& info) = 0;

			boost::signal<void (boost::optional<DNSSDServiceID>)> onRegisterFinished;
	};
}
