/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <SwifTools/Idle/IdleDetector.h>

namespace Swift {
	class IdleQuerier;
	class TimerFactory;
	class Timer;

	class ActualIdleDetector : public IdleDetector, public boost::bsignals::trackable {
		public:
			ActualIdleDetector(IdleQuerier*, TimerFactory*, int refreshRateMilliseconds);
			~ActualIdleDetector();

		private:
			void handleTimerTick();

		private:
			IdleQuerier* querier;
			boost::shared_ptr<Timer> timer;
	};
}
