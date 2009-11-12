#include "Swiften/Network/BoostTimer.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

BoostTimer::BoostTimer(int milliseconds, boost::asio::io_service* service) :
		timeout(milliseconds), timer(*service) {
}

void BoostTimer::start() {
	timer.expires_from_now(boost::posix_time::milliseconds(timeout));
	timer.async_wait(boost::bind(&BoostTimer::handleTimerTick, shared_from_this(), boost::asio::placeholders::error));
}

void BoostTimer::stop() {
	timer.cancel();
}

void BoostTimer::handleTimerTick(const boost::system::error_code& error) {
	if (error) {
		assert(error == boost::asio::error::operation_aborted);
	}
	else {
		MainEventLoop::postEvent(boost::bind(boost::ref(onTick)), shared_from_this());
		timer.expires_from_now(boost::posix_time::milliseconds(timeout));
		timer.async_wait(boost::bind(&BoostTimer::handleTimerTick, shared_from_this(), boost::asio::placeholders::error));
	}
}

}
