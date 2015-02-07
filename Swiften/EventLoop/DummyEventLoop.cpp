/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/DummyEventLoop.h>

#include <iostream>

namespace Swift {

DummyEventLoop::DummyEventLoop() {
}

DummyEventLoop::~DummyEventLoop() {
	boost::lock_guard<boost::mutex> lock(eventsMutex_);
	if (!events_.empty()) {
		std::cerr << "DummyEventLoop: Unhandled events at destruction time" << std::endl;
	}
	events_.clear();
}


}
