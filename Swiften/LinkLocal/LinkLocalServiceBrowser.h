#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"

namespace Swift {
	class HostAddress;

	class LinkLocalServiceBrowser {
		public:
			LinkLocalServiceBrowser(boost::shared_ptr<DNSSDQuerier> querier);

			std::vector<LinkLocalService> getServices() const;

			boost::signal<void (const DNSSDServiceID&)> onServiceAdded;
			boost::signal<void (const DNSSDServiceID&)> onServiceChanged;
			boost::signal<void (const DNSSDServiceID&)> onServiceRemoved;

		private:
			//void handleServiceAdded(const DNSSDServiceID&);
			//void handleServiceRemoved(const DNSSDServiceID&);
			//void handleServiceResolved(const DNSSDServiceID& service, const DNSSDService::ResolveResult& result);

		private:
			boost::shared_ptr<DNSSDQuerier> querier;
			//typedef std::map<DNSSDServiceID, DNSSDService::ResolveResult> ServiceMap;
			//ServiceMap services;
	};
}
