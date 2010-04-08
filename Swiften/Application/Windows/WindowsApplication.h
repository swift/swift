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

			boost::filesystem::path getSettingsDir() const {
				char* appDirRaw = getenv("APPDATA");
				boost::filesystem::path result(boost::filesystem::path(appDirRaw) / getName().getUTF8String());
				boost::filesystem::create_directory(result);
				return result;
			}

			boost::filesystem::path getHomeDir() const {
				//FIXME: This should be My Documents 
				
				char* homeDirRaw = getenv("USERPROFILE");
				return boost::filesystem::path(homeDirRaw);
			}

		private:
			NullApplicationMessageDisplay messageDisplay_;
	};
}

#endif
