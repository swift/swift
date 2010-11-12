/*
 * Copyright (c) 2010 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swiften/EventLoop/SingleThreadedEventLoop.h"

#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/Base/foreach.h"


namespace Swift {

SingleThreadedEventLoop::SingleThreadedEventLoop() 
: shouldShutDown_(false)
{
}

SingleThreadedEventLoop::~SingleThreadedEventLoop() {
	if (!events_.empty()) {
		std::cerr << "Warning: Pending events in SingleThreadedEventLoop at destruction time." << std::endl;
	}
}

void SingleThreadedEventLoop::waitForEvents() {
	boost::unique_lock<boost::mutex> lock(eventsMutex_);
	while (events_.size() == 0 && !shouldShutDown_) {
		eventsAvailable_.wait(lock);
	}

	if (shouldShutDown_)
		throw EventLoopCanceledException();
}

void SingleThreadedEventLoop::handleEvents() {
	// Make a copy of the list of events so we don't block any threads that post 
	// events while we process them.
	std::vector<Event> events;
	{
		boost::unique_lock<boost::mutex> lock(eventsMutex_);
		events.swap(events_);
	}
	
	// Loop through all the events and handle them
	foreach(const Event& event, events) {
		handleEvent(event);
	}
}

void SingleThreadedEventLoop::stop() {
	boost::unique_lock<boost::mutex> lock(eventsMutex_);
	shouldShutDown_ = true;
	eventsAvailable_.notify_one();
}

void SingleThreadedEventLoop::post(const Event& event) {
	boost::lock_guard<boost::mutex> lock(eventsMutex_);
	events_.push_back(event);
	eventsAvailable_.notify_one();
}

} // namespace Swift
