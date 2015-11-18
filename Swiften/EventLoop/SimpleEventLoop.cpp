/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/SimpleEventLoop.h>

#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>

namespace Swift {

SimpleEventLoop::SimpleEventLoop() : isRunning_(true), eventAvailable_(false) {
}

SimpleEventLoop::~SimpleEventLoop() {
}

void SimpleEventLoop::doRun(bool breakAfterEvents) {
	while (isRunning_) {
		{
			boost::unique_lock<boost::mutex> lock(eventAvailableMutex_);
			while (!eventAvailable_) {
				eventAvailableCondition_.wait(lock);
			}

			eventAvailable_ = false;
		}
		runOnce();
		if (breakAfterEvents) {
			return;
		}
	}
}

void SimpleEventLoop::runOnce() {
	handleNextEvent();
}

void SimpleEventLoop::stop() {
	postEvent(boost::bind(&SimpleEventLoop::doStop, this));
}

void SimpleEventLoop::doStop() {
	isRunning_ = false;
}

void SimpleEventLoop::eventPosted() {
	{
		boost::unique_lock<boost::mutex> lock(eventAvailableMutex_);
		eventAvailable_ = true;
	}
	eventAvailableCondition_.notify_one();
}


}
