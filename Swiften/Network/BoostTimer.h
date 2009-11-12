#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/Network/Timer.h"

namespace Swift {
	class BoostTimer : public Timer, public EventOwner, public boost::enable_shared_from_this<BoostTimer> {
		public:
			BoostTimer(int milliseconds, boost::asio::io_service* service);

			virtual void start();
			virtual void stop();

		private:
			void handleTimerTick(const boost::system::error_code& error);

		private:
			int timeout;
			boost::asio::deadline_timer timer;
	};
}
