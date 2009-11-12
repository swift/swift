#pragma once

#include <boost/signals.hpp>

namespace Swift {
	class Timer {
		public:
			virtual ~Timer();

			virtual void start() = 0;
			virtual void stop() = 0;

			boost::signal<void ()> onTick;
	};
}
