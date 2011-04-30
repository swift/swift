/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Application/WindowsApplicationPathProvider.h>

#include <windows.h>

namespace Swift {

WindowsApplicationPathProvider::WindowsApplicationPathProvider(const std::string& name) : ApplicationPathProvider(name) {
	resourceDirs.push_back(getExecutableDir());
	resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
}

}
