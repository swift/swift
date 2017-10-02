/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Storage.h>
#include <Swiften/Parser/PayloadParsers/PrivateStorageParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class PrivateStorageParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PrivateStorageParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testParse_NoPayload);
        CPPUNIT_TEST(testParse_MultiplePayloads);
        CPPUNIT_TEST_SUITE_END();

    public:
        PrivateStorageParserTest() {}

        void testParse() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns='jabber:iq:private'>"
                    "<storage xmlns='storage:bookmarks'>"
                        "<conference name='Swift' jid='swift@rooms.swift.im'>"
                            "<nick>Alice</nick>"
                        "</conference>"
                    "</storage>"
                "</query>"));

            std::shared_ptr<PrivateStorage> payload = std::dynamic_pointer_cast<PrivateStorage>(parser.getPayload());
            CPPUNIT_ASSERT(payload);
            std::shared_ptr<Storage> storage = std::dynamic_pointer_cast<Storage>(payload->getPayload());
            CPPUNIT_ASSERT(storage);
            CPPUNIT_ASSERT_EQUAL(std::string("Alice"), storage->getRooms()[0].nick);
            CPPUNIT_ASSERT_EQUAL(JID("swift@rooms.swift.im"), storage->getRooms()[0].jid);
        }

        void testParse_NoPayload() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse("<query xmlns='jabber:iq:private'/>"));

            std::shared_ptr<PrivateStorage> payload = std::dynamic_pointer_cast<PrivateStorage>(parser.getPayload());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT(!payload->getPayload());
        }

        void testParse_MultiplePayloads() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns='jabber:iq:private'>"
                    "<storage xmlns='storage:bookmarks'>"
                        "<conference name='Swift' jid='swift@rooms.swift.im'>"
                            "<nick>Alice</nick>"
                        "</conference>"
                    "</storage>"
                    "<storage xmlns='storage:bookmarks'>"
                        "<conference name='Swift' jid='swift@rooms.swift.im'>"
                            "<nick>Rabbit</nick>"
                        "</conference>"
                    "</storage>"
                "</query>"));

            std::shared_ptr<PrivateStorage> payload = std::dynamic_pointer_cast<PrivateStorage>(parser.getPayload());
            CPPUNIT_ASSERT(payload);
            std::shared_ptr<Storage> storage = std::dynamic_pointer_cast<Storage>(payload->getPayload());
            CPPUNIT_ASSERT(storage);
            CPPUNIT_ASSERT_EQUAL(std::string("Rabbit"), storage->getRooms()[0].nick);
        }

        void testParse_UnsupportedPayload() {
            PayloadParserFactoryCollection factories;
            PrivateStorageParser testling(&factories);
            PayloadParserTester parser(&testling);

            CPPUNIT_ASSERT(parser.parse(
                "<query xmlns='jabber:iq:private'>"
                    "<foo>Bar</foo>"
                "</query>"));

            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<PrivateStorage>(testling.getPayload())->getPayload());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PrivateStorageParserTest);
