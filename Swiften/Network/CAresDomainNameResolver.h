/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <ares.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <list>

#include <Swiften/Network/DomainNameResolver.h>

namespace Swift {
	class CAresQuery;

	class CAresDomainNameResolver : public DomainNameResolver {
		public:
			CAresDomainNameResolver();
			~CAresDomainNameResolver();

			virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const std::string& name);
			virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const std::string& name);

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
