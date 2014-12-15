/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>

#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/BoostTimer.h>

namespace Swift {
	class BoostTimer;
	class EventLoop;

	class BoostTimerFactory : public TimerFactory {
		public:
			BoostTimerFactory(boost::shared_ptr<boost::asio::io_service>, EventLoop* eventLoop);

			virtual boost::shared_ptr<Timer> createTimer(int milliseconds);

		private:
			boost::shared_ptr<boost::asio::io_service> ioService;
			EventLoop* eventLoop;
	};
}
