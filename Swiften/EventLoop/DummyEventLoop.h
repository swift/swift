#pragma once

#include <deque>
#include <iostream>
#include <boost/function.hpp>

#include "Swiften/EventLoop/EventLoop.h"
#include "Swiften/Base/foreach.h"

namespace Swift {
	class DummyEventLoop : public EventLoop {
		public:
			DummyEventLoop() {
			}

			~DummyEventLoop() {
				if (!events_.empty()) {
					std::cerr << "DummyEventLoop: Unhandled events at destruction time" << std::endl;
				}
				events_.clear();
			}

			void processEvents() {
				while (!events_.empty()) {
					handleEvent(events_[0]);
					events_.pop_front();
				}
			}

			bool hasEvents() {
				return events_.size() > 0;
			}

			virtual void post(const Event& event) {
				events_.push_back(event);
			}

		private:
			std::deque<Event> events_;
	};
}
