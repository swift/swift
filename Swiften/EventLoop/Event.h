/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class Event {
		public:
			Event(boost::shared_ptr<EventOwner> owner, const boost::function<void()>& callback) :
					owner(owner), callback(callback) {
			}

			bool operator==(const Event& o) const {
				return o.id == id;
			}

			unsigned int id;
			boost::shared_ptr<EventOwner> owner;
			boost::function<void()> callback;
	};
}
