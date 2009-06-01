#ifndef SWIFTEN_MacOSXApplicationMessageDisplay_H
#define SWIFTEN_MacOSXApplicationMessageDisplay_H

#include "Swiften/Application/ApplicationMessageDisplay.h"

namespace Swift {
	class String;

	class MacOSXApplicationMessageDisplay : public ApplicationMessageDisplay {
		public:
			MacOSXApplicationMessageDisplay();

			void setMessage(const String& label);
	};
}

#endif
