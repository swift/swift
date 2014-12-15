/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class SWIFTEN_API SimpleEventLoop : public EventLoop {
		public:
			SimpleEventLoop();
			~SimpleEventLoop();

			void run() {
				doRun(false);
			}

			void runUntilEvents() {
				doRun(true);
			}

			void runOnce();

			void stop();

			virtual void post(const Event& event);

		private:
			void doRun(bool breakAfterEvents);
			void doStop();

		private:
			bool isRunning_;
			std::vector<Event> events_;
			boost::mutex eventsMutex_;
			boost::condition_variable eventsAvailable_;
	};
}
