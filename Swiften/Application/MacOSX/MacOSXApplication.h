#ifndef SWIFTEN_MacOSXApplication_H
#define SWIFTEN_MacOSXApplication_H

#include "Swiften/Application/Application.h"
#include "Swiften/Application/MacOSX/MacOSXApplicationMessageDisplay.h"

namespace Swift {
	class ApplicationMessageDisplay;

	class MacOSXApplication : public Application {
		public:
			MacOSXApplication(const String& name);

			virtual boost::filesystem::path getHomeDir() const;
			virtual ApplicationMessageDisplay* getApplicationMessageDisplay();
			boost::filesystem::path getSettingsDir() const;

		private:
			MacOSXApplicationMessageDisplay messageDisplay_;
	};
}

#endif
