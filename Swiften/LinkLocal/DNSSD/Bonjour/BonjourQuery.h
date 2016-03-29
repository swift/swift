/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <dns_sd.h>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
	class BonjourQuerier;
	class EventLoop;

	class BonjourQuery : 
			public EventOwner,
			public boost::enable_shared_from_this<BonjourQuery> {
		public:
			BonjourQuery(boost::shared_ptr<BonjourQuerier>, EventLoop* eventLoop);
			virtual ~BonjourQuery();
			
			void processResult();
			int getSocketID() const;

		protected:
			void run();
			void finish();
		
		protected:
			EventLoop* eventLoop;
			boost::shared_ptr<BonjourQuerier> querier;
			mutable boost::mutex sdRefMutex;
			DNSServiceRef sdRef;
	};
}
