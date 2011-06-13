/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/EventLoop/SimpleEventLoop.h>

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/Base/foreach.h>


namespace Swift {

void nop() {}

SimpleEventLoop::SimpleEventLoop() : isRunning_(true) {
}

SimpleEventLoop::~SimpleEventLoop() {
	if (!events_.empty()) {
		std::cerr << "Warning: Pending events in SimpleEventLoop at destruction time" << std::endl;
	}
}

void SimpleEventLoop::doRun(bool breakAfterEvents) {
	while (isRunning_) {
		std::vector<Event> events;
		{
			boost::unique_lock<boost::mutex> lock(eventsMutex_);
			while (events_.empty()) {
				eventsAvailable_.wait(lock);
			}
			events.swap(events_);
		}
		foreach(const Event& event, events) {
			handleEvent(event);
		}
		if (breakAfterEvents) {
			return;
		}
	}
}

void SimpleEventLoop::runOnce() {
	std::vector<Event> events;
	{
		boost::unique_lock<boost::mutex> lock(eventsMutex_);
		events.swap(events_);
	}
	foreach(const Event& event, events) {
		handleEvent(event);
	}
}

void SimpleEventLoop::stop() {
	postEvent(boost::bind(&SimpleEventLoop::doStop, this));
}

void SimpleEventLoop::doStop() {
	isRunning_ = false;
}

void SimpleEventLoop::post(const Event& event) {
	{
		boost::lock_guard<boost::mutex> lock(eventsMutex_);
		events_.push_back(event);
	}
	eventsAvailable_.notify_one();
}


}
