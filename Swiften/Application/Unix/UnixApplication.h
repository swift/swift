#ifndef SWIFTEN_UnixApplication_H
#define SWIFTEN_UnixApplication_H

#include "Swiften/Application/Application.h"
#include "Swiften/Application/NullApplicationMessageDisplay.h"

namespace Swift {
	class UnixApplication : public Application {
		public:
			UnixApplication(const String& name) : Application(name) {
			}

		private:
			virtual ApplicationMessageDisplay* getApplicationMessageDisplay() {
				return  &messageDisplay_;
			}

			boost::filesystem::path getSettingsDir() const {
				boost::filesystem::path result(getHomeDir() / ("." + getName().getLowerCase().getUTF8String()));
				boost::filesystem::create_directory(result);
				return result;
			}

		private:
			NullApplicationMessageDisplay messageDisplay_;
	};
}

#endif
