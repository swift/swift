#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/LinkLocal/DNSSDBrowseQuery.h"
#include "Swiften/LinkLocal/DNSSDPublishQuery.h"
#include "Swiften/LinkLocal/BonjourQuery.h"

namespace Swift {
	class LinkLocalServiceInfo;

	class BonjourQuerier : 
			public boost::enable_shared_from_this<BonjourQuerier>, 
			public EventOwner {
		public:
			BonjourQuerier();
			~BonjourQuerier();

			boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
			boost::shared_ptr<DNSSDPublishQuery> createPublishQuery(const String& name, int port, const LinkLocalServiceInfo& info);

			void start();
			void stop();

		private:
			friend class BonjourQuery;

			void addRunningQuery(boost::shared_ptr<BonjourQuery>);
			void removeRunningQuery(boost::shared_ptr<BonjourQuery>);
		
		private:
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
