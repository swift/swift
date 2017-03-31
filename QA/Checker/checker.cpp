/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>

#include <gtest/gtest.h>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/TextTestResult.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <Swiften/Base/Log.h>

using Swift::Log;

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool outputXML = false;

    Log::setLogLevel(Swift::Log::error);

    // Parse parameters
    std::vector<std::string> testsToRun;
    for (int i = 1; i < argc; ++i) {
        std::string param(argv[i]);
        if (param == "--verbose") {
            verbose = true;
        }
        else if (param == "--xml") {
            outputXML = true;
        }
        else if (param == "--debug") {
            Swift::Log::setLogLevel(Swift::Log::debug);
        }
        else {
            testsToRun.push_back(param);
        }
    }

    if (testsToRun.empty()) {
        testsToRun.push_back("");
    }

    // generate output filenames for XML test output
    std::string gtestOutputFilename;
    std::string cppunitOutputFilename;

    if (outputXML) {
        auto programName = std::string(argv[0]);

        std::stringstream outFileStringStreamGTest("");
        outFileStringStreamGTest << "xml:" << programName << "-report.gtest.xml";
        gtestOutputFilename = outFileStringStreamGTest.str();

        std::stringstream outFileStringStreamCppUnit("");
        outFileStringStreamCppUnit << programName << "-report.cppunit.xml";
        cppunitOutputFilename = outFileStringStreamCppUnit.str();
    }

    if (outputXML && (std::getenv("GTEST_OUTPUT") == nullptr)) {
        ::testing::GTEST_FLAG(output) = gtestOutputFilename;
    }

    // Google Test might throw an exception in an anonymous namespace. Exiting
    // due to uncaught execption is fine here.
    // coverity[fun_call_w_exception]
    ::testing::InitGoogleTest(&argc, argv);

    // Set up the listeners
    CppUnit::TestResult controller;

    CppUnit::TestResultCollector result;
    controller.addListener(&result);

    CppUnit::TextTestProgressListener progressListener;
    CppUnit::BriefTestProgressListener verboseListener;
    if (!outputXML) {
        if (verbose) {
            controller.addListener(&verboseListener);
        }
        else {
            controller.addListener(&progressListener);
        }
    }

    // Run the tests
    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    for (std::vector<std::string>::const_iterator i = testsToRun.begin(); i != testsToRun.end(); ++i) {
        try {
            runner.run(controller, *i);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return -1;
        }
    }

    // Output the results
    if (outputXML) {
        std::ofstream cppUnitXUnitOutputFile;
        cppUnitXUnitOutputFile.open(cppunitOutputFilename, std::ofstream::out | std::ofstream::trunc);
        if (cppUnitXUnitOutputFile.is_open()) {
            CppUnit::XmlOutputter outputter(&result, cppUnitXUnitOutputFile);
            outputter.write();
        }
        else {
            std::cerr << "Failed to overwrite " << cppunitOutputFilename << " output file." << std::endl;
            return 1;
        }
    }
    else {
        CppUnit::TextOutputter outputter(&result, std::cerr);
        outputter.write();
    }

    auto googleTestWasSuccessful = false;
    try {
        googleTestWasSuccessful = RUN_ALL_TESTS() == 0 ? true : false;
    } catch (const ::testing::internal::GoogleTestFailureException& e) {
        googleTestWasSuccessful = false;
        SWIFT_LOG(error) << "GoogleTestFailureException was thrown: " << e.what() << std::endl;
    }

    auto cppUnitWasSuccessful = result.wasSuccessful() ? true : false;

    return (googleTestWasSuccessful && cppUnitWasSuccessful) ? 0 : 1;
}
