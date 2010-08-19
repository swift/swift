/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_WindowsApplication_H
#define SWIFTEN_WindowsApplication_H

#include "Swiften/Application/Application.h"
#include "Swiften/Application/NullApplicationMessageDisplay.h"

namespace Swift {
	class WindowsApplication : public Application {
		public:
			WindowsApplication(const String& name) : Application(name) {
			}

			virtual ApplicationMessageDisplay* getApplicationMessageDisplay() {
				return  &messageDisplay_;
			}

		private:
			NullApplicationMessageDisplay messageDisplay_;
	};
}

#endif
