/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <list>
#include <deque>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/Event.h>

namespace Swift {
	class EventOwner;

	class SWIFTEN_API EventLoop {
		public:
			EventLoop();
			virtual ~EventLoop();

			void postEvent(boost::function<void ()> event, boost::shared_ptr<EventOwner> owner = boost::shared_ptr<EventOwner>());
			void removeEventsFromOwner(boost::shared_ptr<EventOwner> owner);

		protected:
			/**
			 * Reimplement this to call handleEvent(event) from the thread in which
			 * the event loop is residing.
			 */
			virtual void post(const Event& event) = 0;

			void handleEvent(const Event& event);

		private:
			boost::mutex eventsMutex_;
			unsigned int nextEventID_;
			std::list<Event> events_;
			bool handlingEvents_;
			std::deque<Event> eventsToHandle_;
	};
}
