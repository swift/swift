/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
