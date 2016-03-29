/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API BoostIOServiceThread {
		public:
			/**
			 * Construct the object.
			 * @param ioService If this optional parameter is provided, the behaviour
			 * of this class changes completely - it no longer spawns its own thread
			 * and instead acts as a simple wrapper of the io_service. Use this if
			 * you are re-using an io_service from elsewhere (particularly if you
		   * are using the BoostASIOEventLoop).
			 */
			BoostIOServiceThread(boost::shared_ptr<boost::asio::io_service> ioService = boost::shared_ptr<boost::asio::io_service>());
			~BoostIOServiceThread();

			boost::shared_ptr<boost::asio::io_service> getIOService() const {
				return ioService_;
			}

		private:
			void doRun();

		private:
			boost::shared_ptr<boost::asio::io_service> ioService_;
			boost::thread* thread_;
	};
}
