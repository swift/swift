#include "Swiften/Network/Timer.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

Timer::Timer(int milliseconds) :
		timeout_(milliseconds), ioService_(0), thread_(0), timer_(0) {
  ioService_ = new boost::asio::io_service();
}

Timer::~Timer() {
	//MainEventLoop::removeEventsFromOwner(shared_from_this());
  ioService_->stop();
  thread_->join();
	delete timer_;
	delete thread_;
	delete ioService_;
}

void Timer::start() {
	thread_ = new boost::thread(boost::bind(&Timer::doStart, shared_from_this()));
}

void Timer::doStart() {
	timer_ = new boost::asio::deadline_timer(*ioService_);
	timer_->expires_from_now(boost::posix_time::milliseconds(timeout_));
	timer_->async_wait(boost::bind(&Timer::handleTimerTick, shared_from_this()));
	ioService_->run();
}

void Timer::handleTimerTick() {
	MainEventLoop::postEvent(boost::bind(boost::ref(onTick)), shared_from_this());
	timer_->expires_from_now(boost::posix_time::milliseconds(timeout_));
	timer_->async_wait(boost::bind(&Timer::handleTimerTick, shared_from_this()));
}

}
