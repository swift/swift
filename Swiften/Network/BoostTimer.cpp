/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/BoostTimer.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {

BoostTimer::BoostTimer(int milliseconds, boost::shared_ptr<boost::asio::io_service> service, EventLoop* eventLoop) :
		timeout(milliseconds), ioService(service), timer(*service), eventLoop(eventLoop) {
}

void BoostTimer::start() {
	timer.expires_from_now(boost::posix_time::milliseconds(timeout));
	timer.async_wait(boost::bind(&BoostTimer::handleTimerTick, shared_from_this(), boost::asio::placeholders::error));
}

void BoostTimer::stop() {
	timer.cancel();
	eventLoop->removeEventsFromOwner(shared_from_this());
}

void BoostTimer::handleTimerTick(const boost::system::error_code& error) {
	if (error) {
		assert(error == boost::asio::error::operation_aborted);
	}
	else {
		eventLoop->postEvent(boost::bind(boost::ref(onTick)), shared_from_this());
	}
}

}
