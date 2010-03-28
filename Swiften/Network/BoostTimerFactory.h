#pragma once

#include <boost/asio.hpp>

#include "Swiften/Network/TimerFactory.h"
#include "Swiften/Network/BoostTimer.h"

namespace Swift {
	class BoostTimer;

	class BoostTimerFactory : public TimerFactory {
		public:
			BoostTimerFactory(boost::asio::io_service*);

			virtual boost::shared_ptr<Timer> createTimer(int milliseconds);

		private:
			boost::asio::io_service* ioService;
	};
}
