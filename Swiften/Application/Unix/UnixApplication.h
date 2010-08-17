/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_UnixApplication_H
#define SWIFTEN_UnixApplication_H

#include "Swiften/Application/Application.h"
#include "Swiften/Application/NullApplicationMessageDisplay.h"

#include <iostream>

namespace Swift {
	class UnixApplication : public Application {
		public:
			UnixApplication(const String& name) : Application(name) {
			}

			virtual ApplicationMessageDisplay* getApplicationMessageDisplay() {
				return  &messageDisplay_;
			}

			virtual boost::filesystem::path getHomeDir() const {
				return boost::filesystem::path(getenv("HOME"));
			}

			boost::filesystem::path getSettingsDir() const {
				boost::filesystem::path result(getHomeDir() / ("." + getName().getLowerCase().getUTF8String()));
				try {
					boost::filesystem::create_directory(result);
				}
				catch (const boost::filesystem::filesystem_error& e) {
					std::cerr << "ERROR: " << e.what() << std::endl;
				}
				return result;
			}

		private:
			NullApplicationMessageDisplay messageDisplay_;
	};
}

#endif
