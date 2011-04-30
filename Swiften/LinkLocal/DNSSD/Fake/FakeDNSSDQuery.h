/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

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
