/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
	class Event {
		public:
			Event(boost::shared_ptr<EventOwner> owner, const boost::function<void()>& callback) : id(~0U), owner(owner), callback(callback) {
			}

			bool operator==(const Event& o) const {
				return o.id == id;
			}

			unsigned int id;
			boost::shared_ptr<EventOwner> owner;
			boost::function<void()> callback;
	};
}

std::ostream& operator<<(std::ostream& os, const Swift::Event& e);
