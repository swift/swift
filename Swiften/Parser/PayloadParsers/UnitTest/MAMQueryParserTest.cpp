/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/PayloadParsers/MAMQueryParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class MAMQueryParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(MAMQueryParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testParse_XEP0313_Example3);
        CPPUNIT_TEST(testParse_XEP0313_Example4);
        CPPUNIT_TEST(testParseEmpty);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParse() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<query queryid=\"id0\" xmlns=\"urn:xmpp:mam:0\" node=\"node1\">"
                    "<x type=\"form\" xmlns=\"jabber:x:data\">"
                        "<field type=\"text-single\" var=\"FORM_TYPE\">"
                            "<value>urn:xmpp:mam:0</value>"
                        "</field>"
                        "<field type=\"text-single\" var=\"start\">"
                            "<value>2010-08-07T00:00:00Z</value>"
                        "</field>"
                    "</x>"
                    "<set xmlns=\"http://jabber.org/protocol/rsm\">"
                        "<max>10</max>"
                    "</set>"
                "</query>"));


            std::shared_ptr<MAMQuery> payload = parser.getPayload<MAMQuery>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(payload->getQueryID());
            CPPUNIT_ASSERT_EQUAL(std::string("id0"), *payload->getQueryID());
            CPPUNIT_ASSERT_EQUAL(std::string("node1"), *payload->getNode());

            CPPUNIT_ASSERT(payload->getForm());
            std::shared_ptr<FormField> fieldType = payload->getForm()->getField("FORM_TYPE");
            CPPUNIT_ASSERT(fieldType);
            CPPUNIT_ASSERT_EQUAL(std::string("urn:xmpp:mam:0"), fieldType->getTextSingleValue());
            std::shared_ptr<FormField> fieldStart = payload->getForm()->getField("start");
            CPPUNIT_ASSERT(fieldStart);
            CPPUNIT_ASSERT_EQUAL(std::string("2010-08-07T00:00:00Z"), fieldStart->getTextSingleValue());

            CPPUNIT_ASSERT(payload->getResultSet());
            std::shared_ptr<ResultSet> resultSet = payload->getResultSet();
            CPPUNIT_ASSERT(resultSet->getMaxItems());
            CPPUNIT_ASSERT_EQUAL(*resultSet->getMaxItems(), 10);
        }

        void testParse_XEP0313_Example3() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns='urn:xmpp:mam:0'>"
                    "<x xmlns='jabber:x:data'>"
                        "<field var='FORM_TYPE'>"
                            "<value>urn:xmpp:mam:0</value>"
                        "</field>"
                        "<field var='with'>"
                            "<value>juliet@capulet.lit</value>"
                        "</field>"
                    "</x>"
                "</query>"));

            std::shared_ptr<MAMQuery> payload = parser.getPayload<MAMQuery>();
            CPPUNIT_ASSERT(!!payload && !!payload->getForm() && !!payload->getForm()->getField("FORM_TYPE") && !!payload->getForm()->getField("with"));
            CPPUNIT_ASSERT_EQUAL(std::string("urn:xmpp:mam:0"), payload->getForm()->getField("FORM_TYPE")->getTextSingleValue());
            CPPUNIT_ASSERT_EQUAL(std::string("juliet@capulet.lit"), payload->getForm()->getField("with")->getTextSingleValue());
        }

        void testParse_XEP0313_Example4() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns='urn:xmpp:mam:0'>"
                    "<x xmlns='jabber:x:data'>"
                        "<field var='FORM_TYPE'>"
                            "<value>urn:xmpp:mam:0</value>"
                        "</field>"
                        "<field var='start'>"
                            "<value>2010-06-07T00:00:00Z</value>"
                        "</field>"
                        "<field var='end'>"
                            "<value>2010-07-07T13:23:54Z</value>"
                        "</field>"
                    "</x>"
                "</query>"));
            std::shared_ptr<MAMQuery> payload = parser.getPayload<MAMQuery>();
            CPPUNIT_ASSERT(!!payload && !!payload->getForm() && !!payload->getForm()->getField("FORM_TYPE") && !!payload->getForm()->getField("start") && !!payload->getForm()->getField("start"));
            CPPUNIT_ASSERT_EQUAL(std::string("urn:xmpp:mam:0"), payload->getForm()->getField("FORM_TYPE")->getTextSingleValue());
            CPPUNIT_ASSERT_EQUAL(std::string("2010-06-07T00:00:00Z"), payload->getForm()->getField("start")->getTextSingleValue());
            CPPUNIT_ASSERT_EQUAL(std::string("2010-07-07T13:23:54Z"), payload->getForm()->getField("end")->getTextSingleValue());
        }

        void testParseEmpty() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<query queryid=\"id0\" xmlns=\"urn:xmpp:mam:0\">"
                "</query>"));

            std::shared_ptr<MAMQuery> payload = parser.getPayload<MAMQuery>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(payload->getQueryID());
            CPPUNIT_ASSERT_EQUAL(std::string("id0"), *payload->getQueryID());
            CPPUNIT_ASSERT(!payload->getForm());
            CPPUNIT_ASSERT(!payload->getResultSet());
        }


};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMQueryParserTest);
