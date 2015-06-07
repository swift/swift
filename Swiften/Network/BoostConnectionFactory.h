/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/BoostConnection.h>

namespace Swift {
	class BoostConnection;

	class SWIFTEN_API BoostConnectionFactory : public ConnectionFactory {
		public:
			BoostConnectionFactory(boost::shared_ptr<boost::asio::io_service>, EventLoop* eventLoop);

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			boost::shared_ptr<boost::asio::io_service> ioService;
			EventLoop* eventLoop;
	};
}
