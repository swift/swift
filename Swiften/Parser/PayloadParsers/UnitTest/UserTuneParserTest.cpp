/*
 * Copyright (c) 2015 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/UserTune.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Parser/PayloadParsers/UserTuneParser.h>

using namespace Swift;

class UserTuneParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(UserTuneParserTest);
        CPPUNIT_TEST(testParse_with_all_variables);
        CPPUNIT_TEST(testParse_with_Some_variables);
        CPPUNIT_TEST_SUITE_END();

    public:
        UserTuneParserTest() {}

        void testParse_with_all_variables() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                                "<tune xmlns=\"http://jabber.org/protocol/tune\">"
                                "<rating>5</rating><title>Minion</title><track>Yellow</track><artist>Ice</artist><URI>Fire</URI><source>Origin</source><length>226</length></tune>"));

            UserTune* payload = dynamic_cast<UserTune*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(5), payload->getRating().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Minion"), payload->getTitle().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Yellow"), payload->getTrack().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Ice"), payload->getArtist().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Fire"), payload->getURI().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Origin"), payload->getSource().get());
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(226), payload->getLength().get());
        }

        void testParse_with_Some_variables() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                                "<tune xmlns=\"http://jabber.org/protocol/tune\">"
                                "<title>Minion</title><track>Yellow</track><source>Origin</source><length>226</length></tune>"));

            UserTune* payload = dynamic_cast<UserTune*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT(!payload->getRating());
            CPPUNIT_ASSERT_EQUAL(std::string("Minion"), payload->getTitle().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Yellow"), payload->getTrack().get());
            CPPUNIT_ASSERT(!payload->getArtist());
            CPPUNIT_ASSERT(!payload->getURI());
            CPPUNIT_ASSERT_EQUAL(std::string("Origin"), payload->getSource().get());
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(226), payload->getLength().get());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(UserTuneParserTest);
