#ifndef SWIFTEN_Priority_H
#define SWIFTEN_Priority_H

#include "Swiften/Elements/Payload.h"

namespace Swift {
	class Priority : public Payload {
		public:
			Priority(int priority = 0) : priority_(priority) {
			}

			void setPriority(int priority) {
				priority_ = priority;
			}

			int getPriority() const {
				return priority_;
			}

		private:
			int priority_;
	};
}

#endif
