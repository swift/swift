/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Parser/PayloadParsers/VCardUpdateParser.h>

using namespace Swift;

class VCardUpdateParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(VCardUpdateParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST_SUITE_END();

    public:
        VCardUpdateParserTest() {}

        void testParse() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<x xmlns='vcard-temp:x:update'>"
                    "<photo>sha1-hash-of-image</photo>"
                "</x>"));

            VCardUpdate* payload = dynamic_cast<VCardUpdate*>(parser.getPayload().get());
            CPPUNIT_ASSERT_EQUAL(std::string("sha1-hash-of-image"), payload->getPhotoHash());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardUpdateParserTest);
