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
	class AvahiQuerier;
	class EventLoop;

	class AvahiQuery : 
			public EventOwner,
			public boost::enable_shared_from_this<AvahiQuery> {
		public:
			AvahiQuery(boost::shared_ptr<AvahiQuerier>, EventLoop* eventLoop);
			virtual ~AvahiQuery();
			
		protected:
			boost::shared_ptr<AvahiQuerier> querier;
			EventLoop* eventLoop;
	};
}

