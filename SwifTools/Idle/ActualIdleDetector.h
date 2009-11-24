#pragma once

#include <boost/shared_ptr.hpp>

#include "SwifTools/Idle/IdleDetector.h"

namespace Swift {
	class IdleQuerier;
	class TimerFactory;
	class Timer;

	class ActualIdleDetector : public IdleDetector, public boost::bsignals::trackable {
		public:
			ActualIdleDetector(IdleQuerier*, TimerFactory*, int refreshRateSeconds);
			~ActualIdleDetector();

		private:
			void handleTimerTick();

		private:
			IdleQuerier* querier;
			bool isIdle;
			boost::shared_ptr<Timer> timer;
	};
}
