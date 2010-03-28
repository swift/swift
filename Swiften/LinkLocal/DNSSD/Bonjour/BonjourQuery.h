#pragma once

#include <dns_sd.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>

#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourQuery : 
			public EventOwner,
			public boost::enable_shared_from_this<BonjourQuery> {
		public:
			BonjourQuery(boost::shared_ptr<BonjourQuerier>);
			virtual ~BonjourQuery();
			
			void processResult();
			int getSocketID() const;

		protected:
			void run();
			void finish();
		
		protected:
			boost::shared_ptr<BonjourQuerier> querier;
			mutable boost::mutex sdRefMutex;
			DNSServiceRef sdRef;
	};
}
