/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/Timer.h>

namespace Swift {
	class EventLoop;

	class BoostTimer : public Timer, public EventOwner, public boost::enable_shared_from_this<BoostTimer> {
		public:
			typedef boost::shared_ptr<BoostTimer> ref;

			static ref create(int milliseconds, boost::shared_ptr<boost::asio::io_service> service, EventLoop* eventLoop) {
				return ref(new BoostTimer(milliseconds, service, eventLoop));
			}

			virtual void start();
			virtual void stop();

		private:
			BoostTimer(int milliseconds, boost::shared_ptr<boost::asio::io_service> service, EventLoop* eventLoop);

			void handleTimerTick(const boost::system::error_code& error);

		private:
			int timeout;
			boost::shared_ptr<boost::asio::io_service> ioService;
			boost::asio::deadline_timer timer;
			EventLoop* eventLoop;
	};
}
