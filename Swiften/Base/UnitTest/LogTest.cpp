/*
 * Copyright (c) 2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <vector>

#include <boost/algorithm/string/predicate.hpp>

#include <gtest/gtest.h>

#include <Swiften/Base/Log.h>

using namespace Swift;

struct LogEntry {
    LogEntry(Log::Severity severity, std::string file, int line, std::string function, std::string message) : severity(severity), file(std::move(file)), line(line), function(std::move(function)), message(std::move(message)) {}

    Log::Severity severity;
    std::string file;
    int line;
    std::string function;
    std::string message;
};

// Helper class to set the logging callback.  Using this class to set it will ensure the
// logCallback is reset to empty (its default state) after each test.
class LogCallbackSetter {
public:
    LogCallbackSetter(Log::Callback callback) {
        Log::setLogCallback(callback);
    }
    ~LogCallbackSetter() {
        Log::setLogCallback({});
    }
};

TEST(LogTest, testCallback) {
    std::vector<LogEntry> logEntries;
    LogCallbackSetter callbackSetter = {[&](Log::Severity severity, const std::string& file, int line, const std::string& function, const std::string& message) {
        logEntries.emplace_back(severity, file, line, function, message);
    }};

    SWIFT_LOG(error) << "An error";
    ASSERT_EQ(1, logEntries.size());
    ASSERT_EQ(Log::error, logEntries[0].severity);
    ASSERT_EQ("An error", logEntries[0].message);
}
