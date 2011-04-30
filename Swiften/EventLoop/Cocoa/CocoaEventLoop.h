/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
