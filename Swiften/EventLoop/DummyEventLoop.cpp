/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/DummyEventLoop.h>

#include <iostream>

namespace Swift {

DummyEventLoop::DummyEventLoop() {
}

DummyEventLoop::~DummyEventLoop() {
	if (!events_.empty()) {
		std::cerr << "DummyEventLoop: Unhandled events at destruction time" << std::endl;
	}
	events_.clear();
}


}
