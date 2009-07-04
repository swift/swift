#ifndef SWIFTEN_WindowsApplication_H
#define SWIFTEN_WindowsApplication_H

#include "Swiften/Application/Application.h"
#include "Swiften/Application/NullApplicationMessageDisplay.h"

namespace Swift {
	class WindowsApplication : public Application {
		public:
			WindowsApplication(const String& name) : Application(name) {
			}

		protected:
			virtual ApplicationMessageDisplay* getApplicationMessageDisplay() {
				return  &messageDisplay_;
			}

			boost::filesystem::path getSettingsDir() const {
				boost::filesystem::path result(getHomeDir() / getName().getUTF8String());
				boost::filesystem::create_directory(result);
				return result;
			}

			boost::filesystem::path getHomeDir() const {
				char* homeDirRaw = getenv("APPDATA");
				if (!homeDirRaw) {
					homeDirRaw = getenv("USERPROFILE");
				}
				return boost::filesystem::path(homeDirRaw);
			}

		private:
			NullApplicationMessageDisplay messageDisplay_;
	};
}

#endif
