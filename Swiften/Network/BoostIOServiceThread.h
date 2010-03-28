#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace Swift {
	class BoostIOServiceThread {
		public:
			BoostIOServiceThread();
			~BoostIOServiceThread();

			boost::asio::io_service& getIOService() {
				return ioService_;
			}

		private:
			void doRun();

		private:
			boost::asio::io_service ioService_;
			boost::thread thread_;
	};
}
