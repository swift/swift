#ifndef SWIFTEN_WindowsApplication_H
#define SWIFTEN_WindowsApplication_H

#include <cassert>

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
				assert(false);
				// FIXME: This is wrong
				boost::filesystem::path result(getHomeDir() / ("." + getName().getUTF8String()));
				boost::filesystem::create_directory(result);
				return result;
			}

		private:
			NullApplicationMessageDisplay messageDisplay_;
	};
}

#endif
