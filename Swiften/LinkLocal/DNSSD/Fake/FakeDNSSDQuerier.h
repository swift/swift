/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include <set>

#include "Swiften/Base/foreach.h"
#include <string>
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {
	class ByteArray;
	class FakeDNSSDQuery;
	class FakeDNSSDBrowseQuery;
	class EventLoop;

	class FakeDNSSDQuerier : 
			public DNSSDQuerier, 
			public EventOwner,
			public boost::enable_shared_from_this<FakeDNSSDQuerier> {
		public:
			FakeDNSSDQuerier(const std::string& domain, EventLoop* eventLoop);
			~FakeDNSSDQuerier();

			void start() {}
			void stop() {}

			void clearAllQueriesEverRun() {
				allQueriesEverRun.clear();
			}

			boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
			boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
					const std::string& name, int port, const ByteArray& info);
			boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
					const DNSSDServiceID&);
			boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
					const std::string& hostname, int interfaceIndex);

			void addRunningQuery(boost::shared_ptr<FakeDNSSDQuery>);
			void removeRunningQuery(boost::shared_ptr<FakeDNSSDQuery>);

			void addService(const DNSSDServiceID& id);
			void removeService(const DNSSDServiceID& id);
			void setServiceInfo(const DNSSDServiceID& id, const DNSSDResolveServiceQuery::Result& info);
			bool isServiceRegistered(const std::string& name, int port, const ByteArray& info);
			void setAddress(const std::string& hostname, boost::optional<HostAddress> address);

			void setBrowseError();
			void setRegisterError();

		public:
			template<typename T>
			std::vector< boost::shared_ptr<T> > getAllQueriesEverRun() const {
				std::vector< boost::shared_ptr<T> > result;
				foreach(const boost::shared_ptr<FakeDNSSDQuery>& query, allQueriesEverRun) {
					if (boost::shared_ptr<T> resultQuery = boost::dynamic_pointer_cast<T>(query)) {
						result.push_back(resultQuery);
					}
				}
				return result;
			}

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
			std::string domain;
			EventLoop* eventLoop;
			std::list< boost::shared_ptr<FakeDNSSDQuery> > runningQueries;
			std::list< boost::shared_ptr<FakeDNSSDQuery> > allQueriesEverRun;
			std::set<DNSSDServiceID> services;
			typedef std::map<DNSSDServiceID,DNSSDResolveServiceQuery::Result> ServiceInfoMap;
			ServiceInfoMap serviceInfo;
			std::map<std::string, boost::optional<HostAddress> > addresses;
	};
}
