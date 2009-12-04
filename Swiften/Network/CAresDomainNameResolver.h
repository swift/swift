#pragma once

#include <ares.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <list>

#include "Swiften/Network/DomainNameResolver.h"

namespace Swift {
	class CAresQuery;

	class CAresDomainNameResolver : public DomainNameResolver {
		public:
			CAresDomainNameResolver();
			~CAresDomainNameResolver();

			virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const String& name);
			virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const String& name);

		private:
			friend class CAresQuery;

			void run();
			void addToQueue(boost::shared_ptr<CAresQuery>);

		private:
			bool stopRequested;
			ares_channel channel;
			boost::thread* thread;
			boost::mutex pendingQueriesMutex;
			std::list< boost::shared_ptr<CAresQuery> > pendingQueries;
	};
}
