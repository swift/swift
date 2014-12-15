/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/PlatformDomainNameQuery.h>
#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/DomainNameAddressQuery.h>

namespace Swift {
	class IDNConverter;	
	class EventLoop;

	class SWIFTEN_API PlatformDomainNameResolver : public DomainNameResolver {
		public:
			PlatformDomainNameResolver(IDNConverter* idnConverter, EventLoop* eventLoop);
			~PlatformDomainNameResolver();

			virtual DomainNameServiceQuery::ref createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain);
			virtual DomainNameAddressQuery::ref createAddressQuery(const std::string& name);

		private:
			void run();
			void addQueryToQueue(PlatformDomainNameQuery::ref);

		private:
			friend class PlatformDomainNameServiceQuery;
			friend class PlatformDomainNameAddressQuery;
			IDNConverter* idnConverter;
			EventLoop* eventLoop;
			bool stopRequested;
			boost::thread* thread;
			std::deque<PlatformDomainNameQuery::ref> queue;
			boost::mutex queueMutex;
			boost::condition_variable queueNonEmpty;
	};
}
