#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"
#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h"

namespace Swift {
	class LinkLocalServiceInfo;

	class BonjourQuerier : 
			public DNSSDQuerier, 
			public boost::enable_shared_from_this<BonjourQuerier> {
		public:
			BonjourQuerier();
			~BonjourQuerier();

			boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
			boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
					const String& name, int port, const LinkLocalServiceInfo& info);
			boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
					const LinkLocalServiceID&);
			boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
					const String& hostname, int interfaceIndex);

			void start();
			void stop();

		private:
			friend class BonjourQuery;

			void addRunningQuery(boost::shared_ptr<BonjourQuery>);
			void removeRunningQuery(boost::shared_ptr<BonjourQuery>);
			void interruptSelect();
			void run();

		private:
			bool stopRequested;
			boost::thread* thread;
			boost::mutex runningQueriesMutex;
			std::list< boost::shared_ptr<BonjourQuery> > runningQueries;
			int interruptSelectReadSocket;
			int interruptSelectWriteSocket;
			boost::condition_variable runningQueriesAvailableEvent;
	};
}
