/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

 // Copyright 2005, Google Inc.
 // All rights reserved.
 //
 // Redistribution and use in source and binary forms, with or without
 // modification, are permitted provided that the following conditions are
 // met:
 //
 //     * Redistributions of source code must retain the above copyright
 // notice, this list of conditions and the following disclaimer.
 //     * Redistributions in binary form must reproduce the above
 // copyright notice, this list of conditions and the following disclaimer
 // in the documentation and/or other materials provided with the
 // distribution.
 //     * Neither the name of Google Inc. nor the names of its
 // contributors may be used to endorse or promote products derived from
 // this software without specific prior written permission.
 //
 // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 // "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 // LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 // A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 // OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 // SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 // LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 // DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 // THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 //
 // Author: wan@google.com (Zhanyong Wan)
 //
 // The Google C++ Testing Framework (Google Test)

#include <QA/Checker/CppUnitTestResultPrinter.h>

namespace testing {

    namespace internal {
        enum GTestColor {
            COLOR_DEFAULT,
            COLOR_RED,
            COLOR_GREEN,
            COLOR_YELLOW
        };
        void ColoredPrintf(GTestColor color, const char* fmt, ...);
    }

using namespace internal;

static std::string plural(const std::string msg1, const std::string msg2, int n) {
    if (n == 1) {
        return msg1;
    }
    return msg2;
}

static std::string PrintTestPartResultToString(const TestPartResult& testPartResult) {
    return (Message() << internal::FormatFileLocation(testPartResult.file_name(), testPartResult.line_number())
        << " " << (testPartResult.type() == TestPartResult::Type::kSuccess ? "Success" : "Failure") << std::endl << testPartResult.message()).GetString();
}

void CppUnitTestResultPrinter::OnTestIterationStart(const UnitTest& unitTest, int iteration) {
    if (GTEST_FLAG(repeat) != 1) {
        std::cout << std::endl << "Repeating all tests (iteration" << iteration + 1 << ") . . ." << std::endl << std::endl;
    }
    const char* const filter = GTEST_FLAG(filter).c_str();
    if (!String::CStringEquals(filter, "*")) {
        ColoredPrintf(COLOR_YELLOW, "Note: %s filter = %s\n", GTEST_NAME_, filter);
    }
    if (GTEST_FLAG(shuffle)) {
        ColoredPrintf(COLOR_YELLOW, "Note: Randomizing tests' orders with a seed of %d .\n", unitTest.random_seed());
    }
    ColoredPrintf(COLOR_GREEN, "[==========] ");
    std::cout << "Running " << unitTest.test_to_run_count() << " " << plural("test", "tests", unitTest.test_to_run_count());
    std::cout << " from " << unitTest.test_case_to_run_count() << " " << plural("test case", "test cases", unitTest.test_case_to_run_count()) << "." << std::endl;
    std::cout.flush();
}

void CppUnitTestResultPrinter::OnTestPartResult(const TestPartResult& testPartResult) {
    if (testPartResult.failed()) {
        assertionsFailures_ << PrintTestPartResultToString(testPartResult) << std::endl;
    }
}

void CppUnitTestResultPrinter::OnTestEnd(const TestInfo& testInfo) {
    if (testInfo.result()->Passed()) {
        ColoredPrintf(COLOR_GREEN, ".");
    }
    else {
        ColoredPrintf(COLOR_RED, "F");
        testFailures_ << std::endl << testInfo.name() << "." << testInfo.test_case_name() << std::endl << assertionsFailures_.rdbuf();
        assertionsFailures_.clear();
    }
}

void CppUnitTestResultPrinter::OnTestIterationEnd(const UnitTest& unitTest, int) {
    ColoredPrintf(COLOR_GREEN, "\n[==========] ");
    std::cout << unitTest.test_to_run_count() << " " << plural("test", "tests", unitTest.test_to_run_count()) << " from " << unitTest.test_case_to_run_count() << " " << plural("test case", "test cases", unitTest.test_case_to_run_count()) << " ran. ";
    if (GTEST_FLAG(print_time)) {
        std::cout << "(" << StreamableToString(unitTest.elapsed_time()).c_str() << " ms total)";
    }
    ColoredPrintf(COLOR_GREEN, "\n[  PASSED  ] ");
    std::cout << unitTest.successful_test_count() << " " << plural("test", "tests", unitTest.successful_test_count()) << "." << std::endl;

    int num_failures = unitTest.failed_test_count();
    if (!unitTest.Passed()) {
        ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
        std::cout << unitTest.failed_test_count() << " "  << plural("test", "tests", unitTest.failed_test_count()) << ", listed below:" << std::endl;
        PrintFailedTests(unitTest);
        std::cout << std::endl << num_failures << " FAILED " << plural("TEST", "TESTS", num_failures) << std::endl;
    }
    int num_disabled = unitTest.reportable_disabled_test_count();
    if (num_disabled && !GTEST_FLAG(also_run_disabled_tests)) {
        if (!num_failures) {
            std::cout << std::endl;
        }
        std::cout << num_disabled << " " << plural("test", "tests", num_disabled) << " were disabled." << std::endl << std::endl;
    }
    std::cout << testFailures_.rdbuf();
    std::cout.flush();
}

// Internal helper for printing the list of failed tests.
void CppUnitTestResultPrinter::PrintFailedTests(const UnitTest& unitTest) {
    if (unitTest.failed_test_count() == 0) {
        return;
    }
    for (int i = 0; i < unitTest.total_test_case_count(); ++i) {
        const TestCase& testCase = *unitTest.GetTestCase(i);
        if (!testCase.should_run() || (testCase.failed_test_count() == 0)) {
            continue;
        }
        for (int j = 0; j < testCase.total_test_count(); ++j) {
            const TestInfo& testInfo = *testCase.GetTestInfo(j);
            if (!testInfo.should_run() || testInfo.result()->Passed()) {
                continue;
            }
            ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
            std::cout << testCase.name() << "." << testInfo.name() << std::endl;
        }
    }
}

}
