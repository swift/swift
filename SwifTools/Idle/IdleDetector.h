#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class IdleDetector {
		public:
			IdleDetector() : idle(false) {}
			virtual ~IdleDetector();

			void setIdleTimeSeconds(int time) {
				idleTimeSeconds = time;
			}

			int getIdleTimeSeconds() const {
				return idleTimeSeconds;
			}

			virtual bool isIdle() const {
				return idle;
			}

			boost::signal<void (bool /* isIdle */)> onIdleChanged;

		protected:
			void setIdle(bool b) {
				if (b != idle) {
					idle = b;
					onIdleChanged(b);
				}
			}

		private:
			int idleTimeSeconds;
			bool idle;
	};
}
