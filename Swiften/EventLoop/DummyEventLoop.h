/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <deque>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class DummyEventLoop : public EventLoop {
		public:
			DummyEventLoop();
			~DummyEventLoop();

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
