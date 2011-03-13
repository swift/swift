/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
