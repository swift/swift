#ifndef SWIFTEN_DummyEventLoop_H
#define SWIFTEN_DummyEventLoop_H

#include <deque>
#include <boost/function.hpp>

#include "Swiften/EventLoop/EventLoop.h"
#include "Swiften/Base/foreach.h"

namespace Swift {
	class DummyEventLoop : public EventLoop {
		public:
			DummyEventLoop() {
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

#endif

