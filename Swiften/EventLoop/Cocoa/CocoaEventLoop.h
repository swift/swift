/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class CocoaEventLoop : public EventLoop {
		public:
			CocoaEventLoop();

			virtual void post(const Event& event);

			using EventLoop::handleEvent;
	};
}
