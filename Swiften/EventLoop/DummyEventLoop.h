/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class SWIFTEN_API DummyEventLoop : public EventLoop {
		public:
			DummyEventLoop();
			virtual ~DummyEventLoop();

			void processEvents() {
				while (!events_.empty()) {
					handleEvent(events_[0]);
					events_.pop_front();
				}
			}

			bool hasEvents() {
				return !events_.empty();
			}

			virtual void post(const Event& event) {
				events_.push_back(event);
			}

		private:
			std::deque<Event> events_;
	};
}
