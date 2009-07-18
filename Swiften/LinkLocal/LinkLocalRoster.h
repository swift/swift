#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "Swiften/LinkLocal/DNSSDService.h"

namespace Swift {
	class HostAddress;

	class LinkLocalRoster {
		public:
			LinkLocalRoster(boost::shared_ptr<DNSSDService> service);

		private:
			void handleStopped(bool);
			void handleServiceRegistered(const DNSSDService::Service& service);
			void handleServiceAdded(const DNSSDService::Service&);
			void handleServiceRemoved(const DNSSDService::Service&);
			void handleServiceResolved(const DNSSDService::Service& service, const DNSSDService::ResolveResult& result);
			void handleHostnameResolved(const String& hostname, const boost::optional<HostAddress>& address);

		private:
			boost::shared_ptr<DNSSDService> dnsSDService;
			boost::optional<DNSSDService::Service> selfService;
	};
}
