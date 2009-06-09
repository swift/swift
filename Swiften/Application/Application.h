#ifndef SWIFTEN_Application_H
#define SWIFTEN_Application_H

#include <boost/filesystem.hpp>

#include "Swiften/Base/String.h"

namespace Swift {
	class ApplicationMessageDisplay;

	class Application {
		public:	
			Application(const String& name);
			virtual ~Application();

			boost::filesystem::path getSettingsFileName() const;
			boost::filesystem::path getHomeDir() const;
			virtual boost::filesystem::path getSettingsDir() const = 0;
			boost::filesystem::path getProfileDir(const String& profile) const;

			const String& getName() const {
				return name_;
			}

			virtual ApplicationMessageDisplay* getApplicationMessageDisplay() = 0;

		private:
			String name_;
	};
}

#endif
