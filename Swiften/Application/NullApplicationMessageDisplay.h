/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
