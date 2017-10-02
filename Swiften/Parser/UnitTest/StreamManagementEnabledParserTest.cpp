/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/StreamManagementEnabledParser.h>
#include <Swiften/Parser/UnitTest/ElementParserTester.h>

using namespace Swift;

class StreamManagementEnabledParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(StreamManagementEnabledParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParse() {
            StreamManagementEnabledParser testling;
            ElementParserTester parser(&testling);

            CPPUNIT_ASSERT(parser.parse(
                "<enabled xmlns=\"urn:xmpp:sm:3\" id=\"some-long-sm-id\" resume=\"true\"/>"));

            std::shared_ptr<StreamManagementEnabled> element = std::dynamic_pointer_cast<StreamManagementEnabled>(testling.getElement());
            CPPUNIT_ASSERT(element->getResumeSupported());
            CPPUNIT_ASSERT_EQUAL(std::string("some-long-sm-id"), element->getResumeID());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamManagementEnabledParserTest);
