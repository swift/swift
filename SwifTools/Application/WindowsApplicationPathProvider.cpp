/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SwifTools/Application/WindowsApplicationPathProvider.h"

#include <windows.h>

#include "Swiften/Base/ByteArray.h"

namespace Swift {

WindowsApplicationPathProvider::WindowsApplicationPathProvider(const String& name) : ApplicationPathProvider(name) {
	resourceDirs.push_back(getExecutableDir());
	resourceDirs.push_back(getExecutableDir() / "../resources"); // Development
}

boost::filesystem::path WindowsApplicationPathProvider::getExecutableDir() const {
	ByteArray data;
	data.resize(2048);
	GetModuleFileName(NULL, data.getData(), data.getSize());
	return boost::filesystem::path(data.toString().getUTF8Data()).parent_path();
}

}
