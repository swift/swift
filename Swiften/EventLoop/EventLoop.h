/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <list>
#include <deque>

#include <Swiften/EventLoop/Event.h>

namespace Swift {
	class EventOwner;
	class EventLoop {
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
			struct HasOwner {
				HasOwner(boost::shared_ptr<EventOwner> owner) : owner(owner) {}
				bool operator()(const Event& event) const { return event.owner == owner; }
				boost::shared_ptr<EventOwner> owner;
			};
			boost::mutex eventsMutex_;
			unsigned int nextEventID_;
			std::list<Event> events_;
			bool handlingEvents_;
			std::deque<Event> eventsToHandle_;
	};
}
