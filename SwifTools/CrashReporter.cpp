/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Base/Path.h>

#include <SwifTools/CrashReporter.h>

#if defined(HAVE_BREAKPAD)

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <memory>

#ifdef SWIFTEN_PLATFORM_MACOSX
#include "client/mac/handler/exception_handler.h"
#endif
#ifdef SWIFTEN_PLATFORM_WINDOWS
#include "client/windows/handler/exception_handler.h"
#endif

#if defined(SWIFTEN_PLATFORM_WINDOWS)
static bool handleDump(const wchar_t* /* dir */, const wchar_t* /* id*/, void* /* context */, EXCEPTION_POINTERS*, MDRawAssertionInfo*, bool /* succeeded */) {
    return false;
}
#else
static bool handleDump(const char* /* dir */, const char* /* id*/, void* /* context */, bool /* succeeded */) {
    return false;
}
#endif

namespace Swift {

struct CrashReporter::Private {
    std::shared_ptr<google_breakpad::ExceptionHandler> handler;
};

CrashReporter::CrashReporter(const boost::filesystem::path& path, const std::string& dumpPrefix) {
    // Create the path that will contain the crash dumps
    if (!boost::filesystem::exists(path)) {
        try {
            boost::filesystem::create_directories(path);
        }
        catch (const boost::filesystem::filesystem_error& e) {
            SWIFT_LOG(error) << "ERROR: " << e.what() << std::endl;
        }
    }

    p = std::make_shared<Private>();
#if defined(SWIFTEN_PLATFORM_WINDOWS)
    // FIXME: Need UTF8 conversion from string to wstring
    std::string pathString = pathToString(path);
    p->handler = std::shared_ptr<google_breakpad::ExceptionHandler>(
            // Not using make_shared, because 'handleDump' seems to have problems with VC2010
            new google_breakpad::ExceptionHandler(
                std::wstring(pathString.begin(), pathString.end()),
                (google_breakpad::ExceptionHandler::FilterCallback) 0,
                handleDump,
                (void*) 0,
                google_breakpad::ExceptionHandler::HANDLER_ALL));
    p->handler->set_dump_filename_prefix(std::wstring(dumpPrefix.begin(), dumpPrefix.end()));
// Turning it off for Mac, because it doesn't really help us
//#elif defined(SWIFTEN_PLATFORM_MACOSX)
//    p->handler = std::make_shared<google_breakpad::ExceptionHandler>(pathToString(path), (google_breakpad::ExceptionHandler::FilterCallback) 0, handleDump, (void*) 0, true, (const char*) 0);
#endif
}

};

#else

// Dummy implementation
namespace Swift {
    CrashReporter::CrashReporter(const boost::filesystem::path&, const std::string&) {}
}

#endif
