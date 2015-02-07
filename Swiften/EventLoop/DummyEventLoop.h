/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class SWIFTEN_API DummyEventLoop : public EventLoop {
		public:
			DummyEventLoop();
			virtual ~DummyEventLoop();

			void processEvents() {
				while (hasEvents()) {
					/* 
					  Creating a copy of the to-be-handled Event object because handling
					  it can result in a DummyEventLoop::post() call.
					  This call would also try to lock the eventsMutex_, resulting in a 
					  deadlock. 
					*/

					eventsMutex_.lock();
					Event eventCopy = events_[0];
					eventsMutex_.unlock();

					handleEvent(eventCopy);

					{
						boost::lock_guard<boost::mutex> lock(eventsMutex_);
						events_.pop_front();
					}
				}
			}

			bool hasEvents() {
				boost::lock_guard<boost::mutex> lock(eventsMutex_);
				return !events_.empty();
			}

			virtual void post(const Event& event) {
				boost::lock_guard<boost::mutex> lock(eventsMutex_);
				events_.push_back(event);
			}

		private:
			boost::mutex eventsMutex_;
			std::deque<Event> events_;
	};
}
