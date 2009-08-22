#ifndef SWIFTEN_SimpleEventLoop_H
#define SWIFTEN_SimpleEventLoop_H

#include <vector>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "Swiften/EventLoop/EventLoop.h"

namespace Swift {
	class SimpleEventLoop : public EventLoop {
		public:
			SimpleEventLoop();

			void run();
			void stop();

			virtual void post(const Event& event);

		private:
			void doStop();

		private:
			bool isRunning_;
			std::vector<Event> events_;
			boost::mutex eventsMutex_;
			boost::condition_variable eventsAvailable_;
	};
}
#endif
