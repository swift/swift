/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cstdio>
#include <functional>
#include <memory>
#include <sstream>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API Log {
        public:
            enum Severity {
                error, warning, info, debug
            };
            using Callback = std::function<void(Severity severity, std::string file, int line, std::string function, std::string message)>;

            Log();
            ~Log();

            std::ostringstream& getStream(
                    Severity severity,
                    std::string severityString,
                    std::string file,
                    int line,
                    std::string function);

            static Severity getLogLevel();
            static void setLogLevel(Severity level);
            static void setLogFile(const std::string& fileName);
            static void setLogCallback(Callback callback);

        private:
            struct LogFileClose {
                void operator()(FILE* p) {
                    if (p) {
                        fclose(p);
                    }
                }
            };
            std::ostringstream stream;
            static std::unique_ptr<FILE, LogFileClose> logfile;
            static Callback logCallback;
            Severity severity_;
            std::string file_;
            int line_;
            std::string function_;
    };
}

#define SWIFT_LOG(severity) \
    if (Log::severity > Log::getLogLevel()) ; \
    else Log().getStream(Log::severity, #severity, __FILE__, __LINE__, __FUNCTION__)

#define SWIFT_LOG_ASSERT(test, severity) \
    if (Log::severity > Log::getLogLevel() || (test)) ; \
    else Log().getStream(Log::severity, #severity, __FILE__, __LINE__, __FUNCTION__) << "Assertion failed: " << #test << ". "
