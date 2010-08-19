/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swiften/Application/Application.h"
#include "Swiften/Application/MacOSX/MacOSXApplicationMessageDisplay.h"
#include "Swiften/Application/MacOSX/MacOSXApplicationInitializer.h"

namespace Swift {
	class ApplicationMessageDisplay;

	class MacOSXApplication : public Application {
		public:
			MacOSXApplication(const String& name);

			ApplicationMessageDisplay* getApplicationMessageDisplay();

		private:
			MacOSXApplicationInitializer initializer_;
			MacOSXApplicationMessageDisplay messageDisplay_;
	};
}
