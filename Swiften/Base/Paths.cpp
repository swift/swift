/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
    size_t size = static_cast<size_t>(readlink("/proc/self/exe", reinterpret_cast<char*>(vecptr(path)), path.size()));
    if (size > 0) {
        path.resize(size);
        return boost::filesystem::path(std::string(reinterpret_cast<const char*>(vecptr(path)), path.size()).c_str()).parent_path();
    }
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
    std::vector<wchar_t> data;
    data.resize(2048);
    GetModuleFileNameW(NULL, vecptr(data), data.size());
    return boost::filesystem::path(
            std::wstring(vecptr(data), data.size())).parent_path();
#endif
    return boost::filesystem::path();
}

}
