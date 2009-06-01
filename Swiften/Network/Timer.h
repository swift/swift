#ifndef SWIFTEN_Timer_H
#define SWIFTEN_Timer_H

#include <boost/asio.hpp>
#include <boost/signals.hpp>
#include <boost/thread.hpp>

namespace Swift {
	class Timer {
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

#endif
