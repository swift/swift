#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include <set>

#include "Swiften/Base/foreach.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"

namespace Swift {
	class LinkLocalServiceInfo;
	class FakeDNSSDQuery;
	class FakeDNSSDBrowseQuery;

	class FakeDNSSDQuerier : 
			public DNSSDQuerier, 
			public EventOwner,
			public boost::enable_shared_from_this<FakeDNSSDQuerier> {
		public:
			FakeDNSSDQuerier();

			boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
			boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
					const String& name, int port, const LinkLocalServiceInfo& info);
			boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
					const DNSSDServiceID&);
			boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
					const String& hostname, int interfaceIndex);

			void addRunningQuery(boost::shared_ptr<FakeDNSSDQuery>);
			void removeRunningQuery(boost::shared_ptr<FakeDNSSDQuery>);

			void addService(const DNSSDServiceID& id);
			void removeService(const DNSSDServiceID& id);
			void setServiceInfo(const DNSSDServiceID& id, const DNSSDResolveServiceQuery::Result& info);

			void setBrowseError();
			void setRegisterError();

		private:
			template<typename T>
			std::vector< boost::shared_ptr<T> > getQueries() const {
				std::vector< boost::shared_ptr<T> > result;
				foreach(const boost::shared_ptr<FakeDNSSDQuery>& query, runningQueries) {
					if (boost::shared_ptr<T> resultQuery = boost::dynamic_pointer_cast<T>(query)) {
						result.push_back(resultQuery);
					}
				}
				return result;
			}

		private:
			std::list< boost::shared_ptr<FakeDNSSDQuery> > runningQueries;
			std::set<DNSSDServiceID> services;
			typedef std::map<DNSSDServiceID,DNSSDResolveServiceQuery::Result> ServiceInfoMap;
			ServiceInfoMap serviceInfo;
	};
}
