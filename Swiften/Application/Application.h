/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"

namespace Swift {
	class ApplicationMessageDisplay;

	class Application {
		public:	
			Application(const String& name);
			virtual ~Application();

			const String& getName() const {
				return name_;
			}

			virtual ApplicationMessageDisplay* getApplicationMessageDisplay() = 0;

		private:
			String name_;
	};
}
