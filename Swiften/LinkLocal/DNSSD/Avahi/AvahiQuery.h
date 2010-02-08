#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class AvahiQuerier;

	class AvahiQuery : 
			public EventOwner,
			public boost::enable_shared_from_this<AvahiQuery> {
		public:
			AvahiQuery(boost::shared_ptr<AvahiQuerier>);
			virtual ~AvahiQuery();
			
		protected:
			boost::shared_ptr<AvahiQuerier> querier;
	};
}

