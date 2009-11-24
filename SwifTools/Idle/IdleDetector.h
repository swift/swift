#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class IdleDetector {
		public:
			virtual ~IdleDetector();

			void setIdleTimeSeconds(int time) {
				idleTimeSeconds = time;
			}

			int getIdleTimeSeconds() const {
				return idleTimeSeconds;
			}

			boost::signal<void (bool /* isIdle */)> onIdleChanged;

		private:
			int idleTimeSeconds;
	};
}
