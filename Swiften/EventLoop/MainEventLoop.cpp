#include "Swiften/EventLoop/MainEventLoop.h"

#include <iostream>
#include <typeinfo>

namespace Swift {

EventLoop* MainEventLoop::getInstance() {
	if (!instance_) {
		std::cerr << "No main event loop instantiated. Please instantiate the appropriate subclass of EventLoop (e.g. SimpleEventLoop, QtEventLoop) at the start of your application." << std::endl;
		exit(-1);
	}
	return instance_;
}

void MainEventLoop::setInstance(EventLoop* loop) {
	assert(!instance_);
	instance_ = loop;
}

void MainEventLoop::resetInstance() {
	assert(instance_);
	instance_ = 0;
}

void MainEventLoop::postEvent(boost::function<void ()> event, boost::shared_ptr<EventOwner> owner) {
	getInstance()->postEvent(event, owner);
}

void MainEventLoop::removeEventsFromOwner(boost::shared_ptr<EventOwner> owner) {
	getInstance()->removeEventsFromOwner(owner);
}

EventLoop* MainEventLoop::instance_ = 0;

}
