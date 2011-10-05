/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/EventLoop/EventLoop.h>

#include <algorithm>
#include <boost/bind.hpp>
#include <iostream>
#include <cassert>

#include <Swiften/Base/Log.h>


namespace Swift {

inline void invokeCallback(const Event& event) {
	try {
		assert(!event.callback.empty());
		event.callback();
	}
	catch (const std::exception& e) {
		std::cerr << "Uncaught exception in event loop: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Uncaught non-exception in event loop" << std::endl;	
	}
}

EventLoop::EventLoop() : nextEventID_(0), handlingEvents_(false) {
}

EventLoop::~EventLoop() {
}

void EventLoop::handleEvent(const Event& event) {
	//SWIFT_LOG(debug) << "Handling event " << event.id << std::endl;

	if (handlingEvents_) {
		// We're being called recursively. Push in the list of events to
		// handle in the parent handleEvent()
		eventsToHandle_.push_back(event);
		return;
	}

	bool doCallback = false;
	{
		boost::lock_guard<boost::mutex> lock(eventsMutex_);
		std::list<Event>::iterator i = std::find(events_.begin(), events_.end(), event);
		if (i != events_.end()) {
			doCallback = true;
			events_.erase(i);
		}
	}
	if (doCallback) {
		handlingEvents_ = true;
		invokeCallback(event);

		// Process events that were passed to handleEvent during the callback
		// (i.e. through recursive calls of handleEvent)
		while (!eventsToHandle_.empty()) {
			Event nextEvent = eventsToHandle_.front();
			eventsToHandle_.pop_front();
			invokeCallback(nextEvent);
		}
		handlingEvents_ = false;
	}
}

void EventLoop::postEvent(boost::function<void ()> callback, boost::shared_ptr<EventOwner> owner) {
	Event event(owner, callback);
	{
		boost::lock_guard<boost::mutex> lock(eventsMutex_);
		event.id = nextEventID_;
		nextEventID_++;
		events_.push_back(event);
	}
	//SWIFT_LOG(debug) << "Posting event " << event.id << std::endl;
	post(event);
}

void EventLoop::removeEventsFromOwner(boost::shared_ptr<EventOwner> owner) {
		boost::lock_guard<boost::mutex> lock(eventsMutex_);
		events_.remove_if(HasOwner(owner));
}

}
