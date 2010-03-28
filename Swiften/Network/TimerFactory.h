#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class Timer;

	class TimerFactory {
		public:
			virtual ~TimerFactory();

			virtual boost::shared_ptr<Timer> createTimer(int milliseconds) = 0;
	};
}
