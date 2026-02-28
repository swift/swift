/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/DiscoInfoParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class DiscoInfoParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(DiscoInfoParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testParse_Node);
        CPPUNIT_TEST(testParse_Form);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParse() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns=\"http://jabber.org/protocol/disco#info\">"
                    "<identity name=\"Swift\" category=\"client\" type=\"pc\" xml:lang=\"en\"/>"
                    "<identity name=\"Vlug\" category=\"client\" type=\"pc\" xml:lang=\"nl\"/>"
                    "<feature var=\"foo-feature\"/>"
                    "<feature var=\"bar-feature\"/>"
                    "<feature var=\"baz-feature\"/>"
                "</query>"));

            DiscoInfo::ref payload = std::dynamic_pointer_cast<DiscoInfo>(parser.getPayload());
            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getIdentities().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("Swift"), payload->getIdentities()[0].getName());
            CPPUNIT_ASSERT_EQUAL(std::string("pc"), payload->getIdentities()[0].getType());
            CPPUNIT_ASSERT_EQUAL(std::string("client"), payload->getIdentities()[0].getCategory());
            CPPUNIT_ASSERT_EQUAL(std::string("en"), payload->getIdentities()[0].getLanguage());
            CPPUNIT_ASSERT_EQUAL(std::string("Vlug"), payload->getIdentities()[1].getName());
            CPPUNIT_ASSERT_EQUAL(std::string("pc"), payload->getIdentities()[1].getType());
            CPPUNIT_ASSERT_EQUAL(std::string("client"), payload->getIdentities()[1].getCategory());
            CPPUNIT_ASSERT_EQUAL(std::string("nl"), payload->getIdentities()[1].getLanguage());
            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(payload->getFeatures().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("foo-feature"), payload->getFeatures()[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("bar-feature"), payload->getFeatures()[1]);
            CPPUNIT_ASSERT_EQUAL(std::string("baz-feature"), payload->getFeatures()[2]);
            CPPUNIT_ASSERT(payload->getNode().empty());
        }

        void testParse_Node() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns=\"http://jabber.org/protocol/disco#info\" node=\"blahblah\">"
                    "<identity name=\"Swift\" category=\"client\" type=\"pc\" xml:lang=\"en\"/>"
                    "<identity name=\"Vlug\" category=\"client\" type=\"pc\" xml:lang=\"nl\"/>"
                    "<feature var=\"foo-feature\"/>"
                    "<feature var=\"bar-feature\"/>"
                    "<feature var=\"baz-feature\"/>"
                "</query>"));

            DiscoInfo::ref payload = std::dynamic_pointer_cast<DiscoInfo>(parser.getPayload());
            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getIdentities().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("Swift"), payload->getIdentities()[0].getName());
            CPPUNIT_ASSERT_EQUAL(std::string("pc"), payload->getIdentities()[0].getType());
            CPPUNIT_ASSERT_EQUAL(std::string("client"), payload->getIdentities()[0].getCategory());
            CPPUNIT_ASSERT_EQUAL(std::string("en"), payload->getIdentities()[0].getLanguage());
            CPPUNIT_ASSERT_EQUAL(std::string("Vlug"), payload->getIdentities()[1].getName());
            CPPUNIT_ASSERT_EQUAL(std::string("pc"), payload->getIdentities()[1].getType());
            CPPUNIT_ASSERT_EQUAL(std::string("client"), payload->getIdentities()[1].getCategory());
            CPPUNIT_ASSERT_EQUAL(std::string("nl"), payload->getIdentities()[1].getLanguage());
            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(payload->getFeatures().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("foo-feature"), payload->getFeatures()[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("bar-feature"), payload->getFeatures()[1]);
            CPPUNIT_ASSERT_EQUAL(std::string("baz-feature"), payload->getFeatures()[2]);
            CPPUNIT_ASSERT_EQUAL(std::string("blahblah"), payload->getNode());
        }

        void testParse_Form() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns=\"http://jabber.org/protocol/disco#info\">"
                    "<feature var=\"foo-feature\"/>"
                    "<x type=\"submit\" xmlns=\"jabber:x:data\">"
                        "<title>Bot Configuration</title>"
                        "<instructions>Hello!</instructions>"
                    "</x>"
                    "<feature var=\"bar-feature\"/>"
                "</query>"));

            DiscoInfo::ref payload = std::dynamic_pointer_cast<DiscoInfo>(parser.getPayload());
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(payload->getExtensions().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("Bot Configuration"), payload->getExtensions()[0]->getTitle());
            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getFeatures().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("foo-feature"), payload->getFeatures()[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("bar-feature"), payload->getFeatures()[1]);
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DiscoInfoParserTest);
