#include "Swiften/Network/Timer.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

Timer::Timer(int milliseconds, boost::asio::io_service* service) :
		timeout(milliseconds), timer(*service) {
}

Timer::~Timer() {
	stop();
}

void Timer::start() {
	timer.expires_from_now(boost::posix_time::milliseconds(timeout));
	timer.async_wait(boost::bind(&Timer::handleTimerTick, shared_from_this(), boost::asio::placeholders::error));
}

void Timer::stop() {
	timer.cancel();
}

void Timer::handleTimerTick(const boost::system::error_code& error) {
	if (error) {
		assert(error == boost::asio::error::operation_aborted);
	}
	else {
		MainEventLoop::postEvent(boost::bind(boost::ref(onTick)), shared_from_this());
		timer.expires_from_now(boost::posix_time::milliseconds(timeout));
		timer.async_wait(boost::bind(&Timer::handleTimerTick, shared_from_this(), boost::asio::placeholders::error));
	}
}

}
