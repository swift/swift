/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/BoostASIOEventLoop.h>

#include <boost/bind.hpp>

namespace Swift {

BoostASIOEventLoop::BoostASIOEventLoop(boost::shared_ptr<boost::asio::io_service> ioService) : ioService_(ioService) {

}

BoostASIOEventLoop::~BoostASIOEventLoop() {

}

void BoostASIOEventLoop::handleASIOEvent() {
	{
		boost::recursive_mutex::scoped_lock lock(isEventInASIOEventLoopMutex_);
		isEventInASIOEventLoop_ = false;
	}
	handleNextEvent();
}

void BoostASIOEventLoop::eventPosted() {
	boost::recursive_mutex::scoped_lock lock(isEventInASIOEventLoopMutex_);
	if (!isEventInASIOEventLoop_) {
		isEventInASIOEventLoop_ = true;
		ioService_->post(boost::bind(&BoostASIOEventLoop::handleASIOEvent, this));
	}
}

}
