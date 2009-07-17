#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/LinkLocal/DNSSDService.h"

namespace Swift {
	class LinkLocalRoster {
		public:
			LinkLocalRoster(boost::shared_ptr<DNSSDService> service);

		private:
			void handleServiceAdded(const DNSSDService::Service&);
			void handleServiceRemoved(const DNSSDService::Service&);
			void handleServiceResolved(const DNSSDService::Service& service, const DNSSDService::ResolveResult& result);
			void handleDNSSDError();
			void handleServiceRegistered(const DNSSDService::Service& service);

		private:
			boost::shared_ptr<DNSSDService> dnsSDService;
			boost::optional<DNSSDService::Service> selfService;
	};
}
