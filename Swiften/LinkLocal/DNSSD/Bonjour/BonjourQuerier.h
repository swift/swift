/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class BonjourQuerier : 
			public DNSSDQuerier, 
			public boost::enable_shared_from_this<BonjourQuerier> {
		public:
			BonjourQuerier(EventLoop* eventLoop);
			~BonjourQuerier();

			boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
			boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
					const std::string& name, int port, const ByteArray& info);
			boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
					const DNSSDServiceID&);
			boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
					const std::string& hostname, int interfaceIndex);

			void start();
			void stop();

		private:
			friend class BonjourQuery;

			void addRunningQuery(boost::shared_ptr<BonjourQuery>);
			void removeRunningQuery(boost::shared_ptr<BonjourQuery>);
			void interruptSelect();
			void run();

		private:
			EventLoop* eventLoop;
			bool stopRequested;
			boost::thread* thread;
			boost::mutex runningQueriesMutex;
			std::list< boost::shared_ptr<BonjourQuery> > runningQueries;
			int interruptSelectReadSocket;
			int interruptSelectWriteSocket;
			boost::condition_variable runningQueriesAvailableEvent;
	};
}
