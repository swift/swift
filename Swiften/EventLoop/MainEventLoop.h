/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MainEventLoop_H
#define SWIFTEN_MainEventLoop_H

#include <boost/function.hpp>

#include "Swiften/EventLoop/Deleter.h"
#include "Swiften/EventLoop/EventLoop.h"

namespace Swift {
	class EventLoop;
	class EventOwner;

	class MainEventLoop {
			friend class EventLoop;

		public:
			/**
			 * Post an event from the given owner to the event loop.
			 * If the owner is destroyed, all events should be removed from the
			 * loop using removeEventsFromOwner().
			 */
			static void postEvent(boost::function<void ()> event, boost::shared_ptr<EventOwner> owner = boost::shared_ptr<EventOwner>());

			static void removeEventsFromOwner(boost::shared_ptr<EventOwner> owner);

			template<typename T>
			static void deleteLater(T* t) {
				getInstance()->postEvent(Deleter<T>(t), 0);
			}

		private:
			static void setInstance(EventLoop*);
			static void resetInstance();
			static EventLoop* getInstance();
		
		private:
			static EventLoop* instance_;
	};
}

#endif
