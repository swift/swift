/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>

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
