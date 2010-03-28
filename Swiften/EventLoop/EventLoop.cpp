#include "Swiften/EventLoop/EventLoop.h"

#include <algorithm>
#include <boost/bind.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

EventLoop::EventLoop() : nextEventID_(0) {
	MainEventLoop::setInstance(this);
}

EventLoop::~EventLoop() {
	MainEventLoop::resetInstance();
}

void EventLoop::handleEvent(const Event& event) {
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
		event.callback();
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
	post(event);
}

void EventLoop::removeEventsFromOwner(boost::shared_ptr<EventOwner> owner) {
		boost::lock_guard<boost::mutex> lock(eventsMutex_);
		events_.remove_if(HasOwner(owner));
}

}
