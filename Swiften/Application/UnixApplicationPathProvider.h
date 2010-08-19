/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Application/ApplicationPathProvider.h"

#include <iostream>

namespace Swift {
	class UnixApplicationPathProvider : public ApplicationPathProvider {
		public:
			UnixApplicationPathProvider(const String& name) : ApplicationPathProvider(name) {
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
	};
}

#endif
