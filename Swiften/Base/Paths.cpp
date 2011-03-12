/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/Paths.h>
#include <Swiften/Base/Platform.h>

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <mach-o/dyld.h>
#elif defined(SWIFTEN_PLATFORM_LINUX)
#include <unistd.h>
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
#include <windows.h>
#endif

#include <Swiften/Base/ByteArray.h>

namespace Swift {

boost::filesystem::path Paths::getExecutablePath() {
#if defined(SWIFTEN_PLATFORM_MACOSX)
	ByteArray path;
	uint32_t size = 4096;
	path.resize(size);
	if (_NSGetExecutablePath(reinterpret_cast<char*>(path.getData()), &size) == 0) {
		return boost::filesystem::path(path.toString().c_str()).parent_path();
	}
#elif defined(SWIFTEN_PLATFORM_LINUX)
	ByteArray path;
	path.resize(4096);
	size_t size = readlink("/proc/self/exe", reinterpret_cast<char*>(path.getData()), path.getSize());
	if (size > 0) {
		path.resize(size);
		return boost::filesystem::path(path.toString().c_str()).parent_path();
	}
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
	ByteArray data;
	data.resize(2048);
	GetModuleFileName(NULL, reinterpret_cast<char*>(data.getData()), data.getSize());
	return boost::filesystem::path(data.toString().c_str()).parent_path();
#endif
	return boost::filesystem::path();
}

}
