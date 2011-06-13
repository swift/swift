/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class BoostIOServiceThread {
		public:
			BoostIOServiceThread();
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
