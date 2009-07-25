#pragma once

#include <boost/signal.hpp>
#include <boost/optional.hpp>

#include "Swiften/Network/HostAddress.h"

namespace Swift {
	class DNSSDResolveHostnameQuery {
		public:
			virtual ~DNSSDResolveHostnameQuery();

			virtual void run() = 0;

			boost::signal<void (const boost::optional<HostAddress>&)> onHostnameResolved;
	};
}
