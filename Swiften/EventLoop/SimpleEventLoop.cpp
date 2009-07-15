#include "Swiften/EventLoop/SimpleEventLoop.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"


namespace Swift {

void nop() {}

SimpleEventLoop::SimpleEventLoop() : isRunning_(true) {
}

void SimpleEventLoop::run() {
	while (isRunning_) {
		std::vector<Event> events;
		{
			boost::unique_lock<boost::mutex> lock(eventsMutex_);
			while (events_.size() == 0) {
				eventsAvailable_.wait(lock);
			}
			events.swap(events_);
		}
		foreach(const Event& event, events) {
			handleEvent(event);
		}
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
