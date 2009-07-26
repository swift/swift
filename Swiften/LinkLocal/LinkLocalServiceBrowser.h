#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class LinkLocalServiceBrowser {
		public:
			LinkLocalServiceBrowser(boost::shared_ptr<DNSSDQuerier> querier);
			~LinkLocalServiceBrowser();

			void start();
			void stop();
			bool isRunning() const;
			bool hasError() const;

			void registerService(const String& name, int port, const LinkLocalServiceInfo& info = LinkLocalServiceInfo());
			void unregisterService();
			bool isRegistered() const;

			std::vector<LinkLocalService> getServices() const;

			boost::signal<void (const DNSSDServiceID&)> onServiceAdded;
			boost::signal<void (const DNSSDServiceID&)> onServiceChanged;
			boost::signal<void (const DNSSDServiceID&)> onServiceRemoved;
			boost::signal<void (bool)> onStopped;

		private:
			void handleServiceAdded(const DNSSDServiceID&);
			void handleServiceRemoved(const DNSSDServiceID&);
			void handleServiceResolved(const DNSSDServiceID& service, const boost::optional<DNSSDResolveServiceQuery::Result>& result);
			void handleRegisterFinished(const boost::optional<DNSSDServiceID>&);
			void handleBrowseError();

		private:
			boost::shared_ptr<DNSSDQuerier> querier;
			boost::shared_ptr<DNSSDBrowseQuery> browseQuery;
			boost::shared_ptr<DNSSDRegisterQuery> registerQuery;
			typedef std::map<DNSSDServiceID, boost::shared_ptr<DNSSDResolveServiceQuery> > ResolveQueryMap;
			ResolveQueryMap resolveQueries;
			typedef std::map<DNSSDServiceID, DNSSDResolveServiceQuery::Result> ServiceMap;
			ServiceMap services;
			bool haveError;
	};
}
