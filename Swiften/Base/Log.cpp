/*
 * Copyright (c) 2010-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Log.h>

#include <cstdio>

#if defined(SWIFT_ANDROID_LOGGING) && defined(__ANDROID__)
#include <android/log.h>
#endif


namespace Swift {

static Log::Severity logLevel = Log::warning;
std::unique_ptr<FILE, Log::LogFileClose> Log::logfile;
Log::Callback Log::logCallback;

Log::Log() {
}

Log::~Log() {
#if defined(SWIFT_ANDROID_LOGGING) && defined(__ANDROID__)
    __android_log_print(ANDROID_LOG_VERBOSE, "Swift", stream.str().c_str(), 1);
#else
    // Using stdio for thread safety (POSIX file i/o calls are guaranteed to be atomic)
    if (logCallback) {
        logCallback(severity_, std::move(file_), line_, std::move(function_), stream.str());
    }
    else {
        stream << std::endl;
        if (logfile) {
            fwrite(stream.str().c_str(), sizeof(char), stream.str().size(), logfile.get());
            fflush(logfile.get());
        }
        else {
            fwrite(stream.str().c_str(), sizeof(char), stream.str().size(), stderr);
            fflush(stderr);
        }
    }
#endif
}

std::ostringstream& Log::getStream(
        Severity severity,
        std::string severityString,
        std::string file,
        int line,
        std::string function) {
    if (logCallback) {
        severity_ = severity;
        file_ = std::move(file);
        line_ = line;
        function_ = std::move(function);
    }
    else {
        stream << "[" << severityString << "] " << file << ":" << line << " " << function << ": ";
    }
    return stream;
}

Log::Severity Log::getLogLevel() {
    return logLevel;
}

void Log::setLogLevel(Severity level) {
    logLevel = level;
}

void Log::setLogFile(const std::string& fileName) {
    if (!fileName.empty()) {
        logfile = std::unique_ptr<FILE, Log::LogFileClose>(fopen(fileName.c_str(), "a"));
    }
}

void Log::setLogCallback(Callback callback) {
    Log::logCallback = callback;
}

}
