#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/LinkLocal/LinkLocalService.h"

namespace Swift {
	class HostAddress;

	class LinkLocalServiceBrowser {
		public:
			LinkLocalServiceBrowser(boost::shared_ptr<DNSSDService> service);

			std::vector<LinkLocalService> getServices() const;

			boost::signal<void (const LinkLocalServiceID&)> onServiceAdded;
			boost::signal<void (const LinkLocalServiceID&)> onServiceChanged;
			boost::signal<void (const LinkLocalServiceID&)> onServiceRemoved;

		private:
			void handleServiceAdded(const LinkLocalServiceID&);
			void handleServiceRemoved(const LinkLocalServiceID&);
			void handleServiceResolved(const LinkLocalServiceID& service, const DNSSDService::ResolveResult& result);

		private:
			boost::shared_ptr<DNSSDService> dnsSDService;
			typedef std::map<LinkLocalServiceID, DNSSDService::ResolveResult> ServiceMap;
			ServiceMap services;
	};
}
