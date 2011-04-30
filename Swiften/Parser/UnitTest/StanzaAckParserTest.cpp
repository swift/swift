/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/StanzaAckParser.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/UnitTest/ElementParserTester.h>

using namespace Swift;

class StanzaAckParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StanzaAckParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParse_Invalid);
		CPPUNIT_TEST(testParse_Empty);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			StanzaAckParser testling;
			ElementParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<a h=\"12\" xmlns=\"urn:xmpp:sm:2\"/>"));

			CPPUNIT_ASSERT(testling.getElementGeneric()->isValid());
			CPPUNIT_ASSERT_EQUAL(12U, testling.getElementGeneric()->getHandledStanzasCount());
		}

		void testParse_Invalid() {
			StanzaAckParser testling;
			ElementParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<a h=\"invalid\" xmlns=\"urn:xmpp:sm:2\"/>"));

			CPPUNIT_ASSERT(!testling.getElementGeneric()->isValid());
		}

		void testParse_Empty() {
			StanzaAckParser testling;
			ElementParserTester parser(&testling);

			CPPUNIT_ASSERT(parser.parse("<a xmlns=\"urn:xmpp:sm:2\"/>"));

			CPPUNIT_ASSERT(!testling.getElementGeneric()->isValid());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StanzaAckParserTest);
