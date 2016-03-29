/*
 * Copyright (c) 2010 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/SingleThreadedEventLoop.h>

#include <iostream>

#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>

namespace Swift {

SingleThreadedEventLoop::SingleThreadedEventLoop() 
: shouldShutDown_(false), eventAvailable_(false)
{
}

SingleThreadedEventLoop::~SingleThreadedEventLoop() {

}

void SingleThreadedEventLoop::waitForEvents() {
	boost::unique_lock<boost::mutex> lock(eventAvailableMutex_);
	while (!eventAvailable_ && !shouldShutDown_) {
		eventAvailableCondition_.wait(lock);
	}

	if (shouldShutDown_)
		throw EventLoopCanceledException();
}

void SingleThreadedEventLoop::handleEvents() {
	{
		boost::lock_guard<boost::mutex> lock(eventAvailableMutex_);
		eventAvailable_ = false;
	}
	handleNextEvents();
}

void SingleThreadedEventLoop::stop() {
	boost::unique_lock<boost::mutex> lock(eventAvailableMutex_);
	shouldShutDown_ = true;
	eventAvailableCondition_.notify_one();
}

void SingleThreadedEventLoop::eventPosted() {
	boost::lock_guard<boost::mutex> lock(eventAvailableMutex_);
	eventAvailable_ = true;
	eventAvailableCondition_.notify_one();
}

} // namespace Swift
