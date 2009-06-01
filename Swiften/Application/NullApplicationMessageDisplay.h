#ifndef SWIFTEN_NullApplicationMessageDisplay_H
#define SWIFTEN_NullApplicationMessageDisplay_H

#include "Swiften/Application/ApplicationMessageDisplay.h"

namespace Swift {
	class NullApplicationMessageDisplay : public ApplicationMessageDisplay {
		public:
			NullApplicationMessageDisplay() {}

			virtual void setMessage(const String&) {
			}
	};
}

#endif
