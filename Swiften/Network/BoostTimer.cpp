/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostTimer.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {

BoostTimer::BoostTimer(int milliseconds, boost::shared_ptr<boost::asio::io_service> service, EventLoop* eventLoop) :
		timeout(milliseconds), ioService(service), eventLoop(eventLoop), shuttingDown(false) {
		timer.reset(new boost::asio::deadline_timer(*service));
}

BoostTimer::~BoostTimer() {
	{
		boost::mutex::scoped_lock lockTimer(timerMutex);
		timer.reset();
	}
}

void BoostTimer::start() {
	{
		boost::mutex::scoped_lock lockTimer(timerMutex);
		shuttingDown = false;
		timer->expires_from_now(boost::posix_time::milliseconds(timeout));
		timer->async_wait(boost::bind(&BoostTimer::handleTimerTick, shared_from_this(), boost::asio::placeholders::error));
	}
}

void BoostTimer::stop() {
	{
		boost::mutex::scoped_lock lockTimer(timerMutex);
		shuttingDown = true;
		timer->cancel();
		eventLoop->removeEventsFromOwner(shared_from_this());
	}
}

void BoostTimer::handleTimerTick(const boost::system::error_code& error) {
	if (error) {
		assert(error == boost::asio::error::operation_aborted);
	}
	else {
		{
			boost::mutex::scoped_lock lockTimer(timerMutex);
			if (shuttingDown) {
				return;
			}
			eventLoop->postEvent(boost::bind(boost::ref(onTick)), shared_from_this());
		}
	}
}

}
