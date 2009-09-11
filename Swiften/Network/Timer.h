#pragma once

#include <boost/asio.hpp>
#include <boost/signals.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class Timer : public EventOwner, public boost::enable_shared_from_this<Timer> {
		public:
			Timer(int milliseconds, boost::asio::io_service* service);
			~Timer();

			void start();
			void stop();

		public:
			boost::signal<void ()> onTick;

		private:
			void handleTimerTick(const boost::system::error_code& error);

		private:
			int timeout;
			boost::asio::deadline_timer timer;
	};
}
