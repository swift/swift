#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"

namespace Swift {
	//class HostAddress;

	class LinkLocalServiceBrowser {
		public:
			LinkLocalServiceBrowser(boost::shared_ptr<DNSSDQuerier> querier);
			~LinkLocalServiceBrowser();

			/*std::vector<LinkLocalService> getServices() const;*/

			boost::signal<void (const DNSSDServiceID&)> onServiceAdded;
			boost::signal<void (const DNSSDServiceID&)> onServiceChanged;
			boost::signal<void (const DNSSDServiceID&)> onServiceRemoved;

		private:
			void handleServiceAdded(const DNSSDServiceID&);
			void handleServiceRemoved(const DNSSDServiceID&);
			void handleServiceResolved(const DNSSDServiceID& service, const boost::optional<DNSSDResolveServiceQuery::Result>& result);

		private:
			boost::shared_ptr<DNSSDQuerier> querier;
			boost::shared_ptr<DNSSDBrowseQuery> browseQuery;
			typedef std::map<DNSSDServiceID, boost::shared_ptr<DNSSDResolveServiceQuery> > ResolveQueryMap;
			ResolveQueryMap resolveQueries;
			typedef std::map<DNSSDServiceID, DNSSDResolveServiceQuery::Result> ServiceMap;
			ServiceMap services;
	};
}
