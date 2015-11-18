/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/thread.hpp>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class CocoaEventLoop : public EventLoop {
		public:
			CocoaEventLoop();
			virtual ~CocoaEventLoop();

			void handleNextCocoaEvent();
			
		protected:
			virtual void eventPosted();

		private:
			bool isEventInCocoaEventLoop_;
			boost::recursive_mutex isEventInCocoaEventLoopMutex_;
	};
}
