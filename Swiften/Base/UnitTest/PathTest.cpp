/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Path.h>
#include <Swiften/Base/Platform.h>

using namespace Swift;

class PathTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PathTest);
        CPPUNIT_TEST(testStringToPath);
        CPPUNIT_TEST(testPathToString);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testStringToPath() {
#ifdef SWIFTEN_PLATFORM_WINDOWS
            CPPUNIT_ASSERT(std::wstring(L"tron\xe7on") == stringToPath("tron\xc3\xa7on").native());
#else
            CPPUNIT_ASSERT_EQUAL(std::string("tron\xc3\xa7on"), stringToPath("tron\xc3\xa7on").native());
#endif
        }

        void testPathToString() {
            CPPUNIT_ASSERT_EQUAL(std::string("tron\xc3\xa7on"), pathToString(stringToPath("tron\xc3\xa7on")));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PathTest);

