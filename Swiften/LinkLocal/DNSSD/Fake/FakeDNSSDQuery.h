/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDQuery : 
			public EventOwner,
			public boost::enable_shared_from_this<FakeDNSSDQuery> {
		public:
			FakeDNSSDQuery(boost::shared_ptr<FakeDNSSDQuerier>);
			virtual ~FakeDNSSDQuery();
			
		protected:
			void run();
			void finish();
		
		protected:
			boost::shared_ptr<FakeDNSSDQuerier> querier;
	};
}
