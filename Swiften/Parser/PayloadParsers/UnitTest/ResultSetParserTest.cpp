/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/ResultSetParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class ResultSetParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(ResultSetParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testParseFirstNoIndex);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParse() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                    "<max>100</max>"
                    "<count>800</count>"
                    "<index>0</index>"
                    "<first index=\"123\">stpeter@jabber.org</first>"
                    "<last>peterpan@neverland.lit</last>"
                    "<before>decaf-badba-dbad1</before>"
                    "<after>09af3-cc343-b409f</after>"
                "</set>"));

            std::shared_ptr<ResultSet> payload = parser.getPayload<ResultSet>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(payload->getMaxItems());
            CPPUNIT_ASSERT_EQUAL(100, *payload->getMaxItems());
            CPPUNIT_ASSERT(payload->getCount());
            CPPUNIT_ASSERT_EQUAL(800, *payload->getCount());
            CPPUNIT_ASSERT(payload->getIndex());
            CPPUNIT_ASSERT_EQUAL(0, *payload->getIndex());
            CPPUNIT_ASSERT(payload->getFirstID());
            CPPUNIT_ASSERT_EQUAL(std::string("stpeter@jabber.org"), *payload->getFirstID());
            CPPUNIT_ASSERT(payload->getFirstIDIndex());
            CPPUNIT_ASSERT_EQUAL(123, *payload->getFirstIDIndex());
            CPPUNIT_ASSERT(payload->getLastID());
            CPPUNIT_ASSERT_EQUAL(std::string("peterpan@neverland.lit"), *payload->getLastID());
            CPPUNIT_ASSERT(payload->getBefore());
            CPPUNIT_ASSERT_EQUAL(std::string("decaf-badba-dbad1"), *payload->getBefore());
            CPPUNIT_ASSERT(payload->getAfter());
            CPPUNIT_ASSERT_EQUAL(std::string("09af3-cc343-b409f"), *payload->getAfter());
        }

        void testParseFirstNoIndex() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                    "<first>stpeter@jabber.org</first>"
                "</set>"));

            std::shared_ptr<ResultSet> payload = parser.getPayload<ResultSet>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(payload->getFirstID());
            CPPUNIT_ASSERT_EQUAL(std::string("stpeter@jabber.org"), *payload->getFirstID());
            CPPUNIT_ASSERT(!payload->getFirstIDIndex());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ResultSetParserTest);
