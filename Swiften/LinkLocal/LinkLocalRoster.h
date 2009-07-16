#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/LinkLocal/DNSSDService.h"

namespace Swift {
	class LinkLocalRoster {
		public:
			LinkLocalRoster(boost::shared_ptr<DNSSDService> service);

		private:
			void handleServiceAdded(const DNSSDService::DiscoveredService&);
			void handleServiceRemoved(const DNSSDService::DiscoveredService&);

		private:
			boost::shared_ptr<DNSSDService> dnsSDService;
	};
}
