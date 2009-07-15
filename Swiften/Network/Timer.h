#pragma once

#include <boost/asio.hpp>
#include <boost/signals.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class Timer : public EventOwner, public boost::enable_shared_from_this<Timer> {
		public:
			Timer(int milliseconds);
			~Timer();

			void start();

		public:
			boost::signal<void ()> onTick;

		private:
			void doStart();
			void handleTimerTick();

		private:
			int timeout_;
			boost::asio::io_service* ioService_;
			boost::thread* thread_;
			boost::asio::deadline_timer* timer_;
	};
}
