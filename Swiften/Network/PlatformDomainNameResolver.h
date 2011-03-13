/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <deque>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/PlatformDomainNameQuery.h>
#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/DomainNameAddressQuery.h>

namespace Swift {
	
	class EventLoop;

	class PlatformDomainNameResolver : public DomainNameResolver {
		public:
			PlatformDomainNameResolver(EventLoop* eventLoop);
			~PlatformDomainNameResolver();

			virtual DomainNameServiceQuery::ref createServiceQuery(const std::string& name);
			virtual DomainNameAddressQuery::ref createAddressQuery(const std::string& name);

		private:
			void run();
			void addQueryToQueue(PlatformDomainNameQuery::ref);

		private:
			friend class PlatformDomainNameServiceQuery;
			friend class PlatformDomainNameAddressQuery;
			EventLoop* eventLoop;
			bool stopRequested;
			boost::thread* thread;
			std::deque<PlatformDomainNameQuery::ref> queue;
			boost::mutex queueMutex;
			boost::condition_variable queueNonEmpty;
	};
}
