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
	if (_NSGetExecutablePath(const_cast<char*>(reinterpret_cast<const char*>(vecptr(path))), &size) == 0) {
		return boost::filesystem::path(std::string(reinterpret_cast<const char*>(vecptr(path)), path.size()).c_str()).parent_path();
	}
#elif defined(SWIFTEN_PLATFORM_LINUX)
	ByteArray path;
	path.resize(4096);
	ssize_t size = readlink("/proc/self/exe", reinterpret_cast<char*>(vecptr(path)), path.size());
	if (size > 0) {
		path.resize(size);
		return boost::filesystem::path(std::string(reinterpret_cast<const char*>(vecptr(path)), path.size()).c_str()).parent_path();
	}
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
	ByteArray data;
	data.resize(2048);
	GetModuleFileName(NULL, reinterpret_cast<char*>(vecptr(data)), data.size());
	return boost::filesystem::path(std::string(reinterpret_cast<const char*>(vecptr(data)), data.size()).c_str()).parent_path();
#endif
	return boost::filesystem::path();
}

}
