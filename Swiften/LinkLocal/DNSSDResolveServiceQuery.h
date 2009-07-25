#pragma once

#include <boost/signal.hpp>
#include <boost/optional.hpp>

#include "Swiften/LinkLocal/LinkLocalServiceID.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class DNSSDResolveServiceQuery {
		public:
			struct Result {
				Result(const String& fullName, const String& host, int port, 
						const LinkLocalServiceInfo& info) : 
							fullName(fullName), host(host), port(port), info(info) {}
				String fullName;
				String host;
				int port;
				LinkLocalServiceInfo info;
			};

			virtual ~DNSSDResolveServiceQuery();

			virtual void start() = 0;
			virtual void stop() = 0;

			boost::signal<void (const boost::optional<Result>&)> onServiceResolved;
	};
}
