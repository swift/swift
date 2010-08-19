/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Application/ApplicationPathProvider.h"

#include <iostream>
#include <unistd.h>

#include "Swiften/Base/ByteArray.h"

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

			virtual boost::filesystem::path getExecutableDir() const {
				ByteArray path;
				path.resize(SSIZE_MAX);
				size_t size = readlink("/proc/self/exe", path.getData(), path.getSize());
				if (size > 0) {
					path.resize(size);
					return boost::filesystem::path(path.toString().getUTF8Data()).parent_path();
				}
				else {
					return boost::filesystem::path();
				}
			}
	};
}

#endif
