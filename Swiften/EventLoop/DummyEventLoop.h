/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class SWIFTEN_API DummyEventLoop : public EventLoop {
		public:
			DummyEventLoop();
			virtual ~DummyEventLoop();

			void processEvents();

			bool hasEvents();

			virtual void eventPosted();

		private:
			bool hasEvents_;
			boost::mutex hasEventsMutex_;
	};
}
