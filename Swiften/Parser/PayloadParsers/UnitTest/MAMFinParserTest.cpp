/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/MAMFin.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class MAMFinParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(MAMFinParserTest);
        CPPUNIT_TEST(testParse_XEP0313_Exmaple1);
        CPPUNIT_TEST(testParse_XEP0313_Exmaple9);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParse_XEP0313_Exmaple1() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<fin xmlns='urn:xmpp:mam:0' queryid='f27' />"));

            std::shared_ptr<MAMFin> payload = parser.getPayload<MAMFin>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT_EQUAL(false, payload->isComplete());
            CPPUNIT_ASSERT_EQUAL(true, payload->isStable());

            boost::optional<std::string> queryID = payload->getQueryID();
            CPPUNIT_ASSERT(queryID);
            CPPUNIT_ASSERT_EQUAL(std::string("f27"), queryID.get());
        }

        void testParse_XEP0313_Exmaple9() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<fin xmlns='urn:xmpp:mam:0' complete='true'>"
                    "<set xmlns='http://jabber.org/protocol/rsm'>"
                        "<first index='0'>23452-4534-1</first>"
                        "<last>390-2342-22</last>"
                        "<count>16</count>"
                    "</set>"
                "</fin>"));

            std::shared_ptr<MAMFin> payload = parser.getPayload<MAMFin>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT_EQUAL(true, payload->isComplete());
            CPPUNIT_ASSERT_EQUAL(true, payload->isStable());

            CPPUNIT_ASSERT(!!payload->getResultSet());
            std::shared_ptr<ResultSet> resultSet = payload->getResultSet();
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMFinParserTest);
