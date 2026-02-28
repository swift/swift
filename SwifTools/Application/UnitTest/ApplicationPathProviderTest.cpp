/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

#include <boost/algorithm/string.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Path.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>

using namespace Swift;

class ApplicationPathProviderTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ApplicationPathProviderTest);
        CPPUNIT_TEST(testGetDataDir);
        CPPUNIT_TEST(testGetExecutableDir);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            testling_ = new PlatformApplicationPathProvider("SwiftTest");
        }

        void tearDown() {
            delete testling_;
        }

        void testGetDataDir() {
            boost::filesystem::path dir = testling_->getDataDir();

            CPPUNIT_ASSERT(boost::filesystem::exists(dir));
            CPPUNIT_ASSERT(boost::filesystem::is_directory(dir));

            boost::filesystem::remove(dir);
        }

        void testGetExecutableDir() {
            boost::filesystem::path dir = testling_->getExecutableDir();
            CPPUNIT_ASSERT(boost::filesystem::is_directory(dir));
            CPPUNIT_ASSERT(boost::ends_with(pathToString(dir), "UnitTest"));
        }

    private:
        ApplicationPathProvider* testling_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ApplicationPathProviderTest);
