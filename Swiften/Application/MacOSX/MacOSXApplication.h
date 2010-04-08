/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MacOSXApplication_H
#define SWIFTEN_MacOSXApplication_H

#include "Swiften/Application/Application.h"
#include "Swiften/Application/MacOSX/MacOSXApplicationMessageDisplay.h"
#include "Swiften/Application/MacOSX/MacOSXApplicationInitializer.h"

namespace Swift {
	class ApplicationMessageDisplay;

	class MacOSXApplication : public Application {
		public:
			MacOSXApplication(const String& name);

			virtual boost::filesystem::path getHomeDir() const;
			virtual ApplicationMessageDisplay* getApplicationMessageDisplay();
			boost::filesystem::path getSettingsDir() const;

		private:
			MacOSXApplicationInitializer initializer_;
			MacOSXApplicationMessageDisplay messageDisplay_;
	};
}

#endif
