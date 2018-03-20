/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cstdio>
#include <memory>
#include <sstream>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API Log {
        public:
            enum Severity {
                error, warning, info, debug
            };

            Log();
            ~Log();

            std::ostringstream& getStream(
                    Severity severity,
                    const std::string& severityString,
                    const std::string& file,
                    int line,
                    const std::string& function);

            static Severity getLogLevel();
            static void setLogLevel(Severity level);
            static void setLogFile(const std::string& fileName);

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
    };
}

#define SWIFT_LOG(severity) \
    if (Log::severity > Log::getLogLevel()) ; \
    else Log().getStream(Log::severity, #severity, __FILE__, __LINE__, __FUNCTION__)

#define SWIFT_LOG_ASSERT(test, severity) \
    if (Log::severity > Log::getLogLevel() || (test)) ; \
    else Log().getStream(Log::severity, #severity, __FILE__, __LINE__, __FUNCTION__) << "Assertion failed: " << #test << ". "
