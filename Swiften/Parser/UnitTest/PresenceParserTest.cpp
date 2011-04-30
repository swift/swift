/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PresenceParser.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/UnitTest/StanzaParserTester.h>

using namespace Swift;

class PresenceParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PresenceParserTest);
		CPPUNIT_TEST(testParse_Available);
		CPPUNIT_TEST(testParse_Unavailable);
		CPPUNIT_TEST(testParse_Subscribe);
		CPPUNIT_TEST(testParse_Subscribed);
		CPPUNIT_TEST(testParse_Unsubscribe);
		CPPUNIT_TEST(testParse_Unsubscribed);
		CPPUNIT_TEST(testParse_Probe);
		CPPUNIT_TEST(testParse_Error);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			factoryCollection_ = new PayloadParserFactoryCollection();
		}

		void tearDown() {
			delete factoryCollection_;
		}

		void testParse_Available() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Available, testling.getStanzaGeneric()->getType());
		}

		void testParse_Unavailable() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence type=\"unavailable\"/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Unavailable, testling.getStanzaGeneric()->getType());
		}

		void testParse_Probe() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence type=\"probe\"/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Probe, testling.getStanzaGeneric()->getType());
		}

		void testParse_Subscribe() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence type=\"subscribe\"/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Subscribe, testling.getStanzaGeneric()->getType());
		}

		void testParse_Subscribed() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence type=\"subscribed\"/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Subscribed, testling.getStanzaGeneric()->getType());
		}

		void testParse_Unsubscribe() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence type=\"unsubscribe\"/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Unsubscribe, testling.getStanzaGeneric()->getType());
		}

		void testParse_Unsubscribed() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence type=\"unsubscribed\"/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Unsubscribed, testling.getStanzaGeneric()->getType());
		}

		void testParse_Error() {
			PresenceParser testling(factoryCollection_);
			StanzaParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<presence type=\"error\"/>"));

			CPPUNIT_ASSERT_EQUAL(Presence::Error, testling.getStanzaGeneric()->getType());
		}

		private:
			PayloadParserFactoryCollection* factoryCollection_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PresenceParserTest);
